#include "client.h"
#include "model.h"
#include "network_utils.h"
#include "server.h"

namespace {
auto server_name = "tcp://188.119.67.234:" + std::to_string(port);

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
        while (my_id == -1) {
        }
        assert(my_id != -1);
        while (true) {
            Message pullRequest =
                Message::create<Message::MessageType::Check>(my_id, 2, 2);
            send(local_socket, std::move(pullRequest));
            auto result = receive(local_socket);
            std::cerr << "client received: " << result.type() << std::endl;
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

Message Client::send_to_server(Message &&msg) {
    auto res = send_and_receive(socket, std::move(msg));
    std::cerr << "res.type() = " << res.type() << std::endl;
    return res;
}

int Client::connect_to_server() {
    return set_connection(socket);
}

void Client::messageAvailable(Message &&msg) noexcept {
    std::unique_lock l(m);
    if (msg.type() == Message::MessageType::Connect ||
        msg.type() == Message::MessageType::AudioSource ||
        msg.type() == Message::MessageType::Move ||
        msg.type() == Message::MessageType::Destroy) {
        auto res = send_to_server(std::move(msg));
        if (res.type() == Message::MessageType::Connect) {
            std::cerr << "new_id = " << res.id() << std::endl;
            my_id = res.id();
            model->get_queue()->putMessage(std::move(res));
        }
    } else {
        assert(false);  // must not be other MessageType from model to client
    }
}

folly::NotificationQueue<Message> *Client::get_queue() {
    return &queue;
}

bool Client::is_ok_connection() {
    std::unique_lock l(m);
    Message request =
        Message::create<Message::MessageType::Check>(model->get_id(), 0, 0);
    Message response = send_and_receive(socket, std::move(request));
    return response.x() == 1;  // crutch
}

Client::~Client() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
    network_thread.detach();
}
