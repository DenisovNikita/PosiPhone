#include "client.h"
#include "model.h"
#include "server.h"

namespace {
auto server_name = "tcp://188.119.67.234:1234";

int set_connection(zmq::socket_t &socket) {
    socket.connect(server_name);
    // fill field server
    return 0;
}

}  // namespace

Client::Client(Model *m)
    : model(m),
      server(nullptr),
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
}

void Client::send_to_server(Message &&msg) {
    // TODO: send msg to client via socket
}

int Client::connect_to_server() {
    return set_connection(socket);
}

void Client::messageAvailable(Message &&msg) noexcept {
    if (msg.type() == Message::MessageType::Connect ||
        msg.type() == Message::MessageType::AudioSource) {
        send_to_server(std::move(msg));
    } else if (msg.type() == Message::MessageType::Create ||
               msg.type() ==
                   Message::MessageType::AudioResult) {  // must be info from
                                                         // another
                                                         // client
        model->get_queue()->putMessage(std::move(msg));
    } else if (msg.type() == Message::MessageType::Move ||
               msg.type() == Message::MessageType::Destroy) {
        if (my_id == msg.id()) {  // msg came from model
            send_to_server(std::move(msg));
        } else {  // msg came from server
            model->get_queue()->putMessage(std::move(msg));
        }
    } else {
        assert(false);  // must not be other MessageType
    }
}

folly::NotificationQueue<Message> *Client::get_queue() {
    return &queue;
}

bool Client::is_ok_connection() {
    //    TODO: Long-polling requests to server
    return 1;
}

Client::~Client() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}
