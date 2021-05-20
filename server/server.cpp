#include "server.h"
#include "client.h"

Server::Server()
    : context(1), socket(context, ZMQ_REP), th("server"), queue(max_size) {
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
    socket.bind("tcp://*:1234");
}

folly::NotificationQueue<Message> *Server::get_queue() {
    return &queue;
}

void Server::messageAvailable(Message &&msg) noexcept {
    if (/*I should send to client*/ 1) {  // TODO: make check condition
        clients[msg.id]->get_queue()->putMessage(std::move(msg));
//        clients[msg.id]->get_queue()->putMessage(
//            std::move(msg));  // Is all ok there?
    } else {
        // TODO: send msg to mixer
    }
}

Server::~Server() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}

int main() {
    Server server;
}
