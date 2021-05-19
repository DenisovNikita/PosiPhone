#include "client.h"
#include "model.h"
#include "server.h"

namespace {
    auto server_name = "tcp://188.119.67.234:1234";

}

Client::Client(Model *m, Server *s) : model(m), server(s),
                                      context(1), socket(context, ZMQ_REQ),
                                      th("client"), queue(max_size) {
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
            [eventBase, this]() { startConsuming(eventBase, &queue); });
}

int Client::connect_to_server() {
    cout << "Connecting to server..."
         << "\n";
    socket.connect(server_name);
    return 0;
}

void Client::messageAvailable(Message &&msg) noexcept {
    if (model->get_id() == msg.id) {// msg from model
        server->get_queue()->putMessage(std::move(msg));
    } else {// msg from server
        model->getCurrentQueue()->putMessage(std::move(msg));
    }
}

folly::NotificationQueue<Message> *Client::get_queue() {
    return &queue;
}

bool Client::is_ok_connection() {
    return 1;
}

Client::~Client() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}
