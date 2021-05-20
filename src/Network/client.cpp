#include "client.h"
//#include "model.h"
#include "server.h"

namespace {
auto server_name = "tcp://188.119.67.234:1234";

}

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
    // TODO: write error messages to users
    socket.connect(server_name);
}

int Client::connect_to_server() {
    cout << "Connecting to server...\n";
    socket.connect(server_name);
    // fill field server
    return 0;
}

void Client::messageAvailable(Message &&msg) noexcept {
    cout << "got " << msg.type() << " message\n";
    //    TODO: parsing all messages type and update corresponding info
    //    if (model->get_id() == msg.id) {  // msg from model
    //        server->get_queue()->putMessage(std::move(msg));
    //    } else {  // msg from server
    //        model->getCurrentQueue()->putMessage(std::move(msg));
    //    }
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
