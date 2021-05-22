#include "client.h"
#include "model.h"
#include "network_utils.h"
#include "server.h"

namespace {
auto server_name = "tcp://188.119.67.234:4444";

int set_connection(zmq::socket_t &socket) {
    socket.connect(server_name);
    return 0;
}

}  // namespace

Client::Client(Model *m)
    : model(m),
      context(1),
      socket(context, ZMQ_REQ),
      th("client"),
      queue(max_size) {
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });

    // TODO: connect from model via Client::connect_to_server() in order to
    //  write error messages to users
    assert(set_connection(socket) == 0);

    network_thread = std::thread([&]() {
        zmq::context_t local_context(1);
        zmq::socket_t local_socket(local_context, ZMQ_REQ);
        local_socket.connect(server_name);
        auto first_message = receive(local_socket);
        while (first_message.type() == Message::MessageType::Empty) {
            first_message = receive(local_socket);
        }
        assert(first_message.type() == Message::MessageType::Connect);
        std::int64_t my_id = first_message.id();
        while (true) {
            Message pullRequest =
                Message::create<Message::MessageType::Check>(my_id, 2, 2);
            send(local_socket, std::move(pullRequest));
            auto result = receive(local_socket);
            if (result.type() == Message::MessageType::Create ||
                result.type() == Message::MessageType::Move ||
                result.type() == Message::MessageType::AudioResult ||
                result.type() == Message::MessageType::Destroy) {
                model->get_queue()->putMessage(std::move(result));
            } else if (result.type() == Message::MessageType::Empty) {
                continue;
            } else {
                assert(false);  // must not be other message types
            }
        }
    });
}

void Client::send_to_server(Message &&msg) {
    send(socket, std::move(msg));  // Is it works? Should be.
}

int Client::connect_to_server() {
    return set_connection(socket);
}

void Client::messageAvailable(Message &&msg) noexcept {
    if (msg.type() == Message::MessageType::Connect ||
        msg.type() == Message::MessageType::AudioSource ||
        msg.type() == Message::MessageType::Move ||
        msg.type() == Message::MessageType::Destroy) {
        send_to_server(std::move(msg));
    } else {
        assert(false);  // must not be other MessageType from model to client
    }
}

folly::NotificationQueue<Message> *Client::get_queue() {
    return &queue;
}

bool Client::is_ok_connection() {
    Message request =
        Message::create<Message::MessageType::Check>(model->get_id(), 0, 0);
    send(socket, std::move(request));
    Message response = receive(socket);
    return response.x() == 1;  // crutch
}

Client::~Client() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
    network_thread.detach();
}
