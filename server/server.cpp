#include "server.h"
#include "client.h"
#include "network_utils.h"

Server::Server()
    : context(1), socket(context, ZMQ_REP), th("server"), queue(max_size) {
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
    socket.bind("tcp://*:" + std::to_string(port));
}

folly::NotificationQueue<Message> *Server::get_queue() {
    return &queue;
}

void Server::messageAvailable(Message &&msg) noexcept {
    if (msg.type() == Message::MessageType::AudioResult) {  // send to one
                                                            // client from mixer
        send_to_one_client(std::move(msg));
    } else {
        // Other MessageType's are not suitable situation for message from
        // mixer to server
        std::cerr << "Received bad message type (to server from mixer) = |"
                  << msg.type() << "|\n";
        assert(false);
    }
}

void Server::send_to_all_clients_except_one(Message &&msg) {
    //    TODO: zmq_router or something else
    for (auto &[id, v] : messages) {
        if (msg.id() != id) {
            v.push_back(msg);
        }
    }
}

void Server::send_to_one_client(Message &&msg) {
    send(socket, std::move(msg));
}

Server::~Server() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}
