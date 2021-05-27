#include "server_network_module.h"
#include "client.h"
#include "network_utils.h"

Server_network_module::Server_network_module()
    : context(IO_THREADS_), socket(context, ZMQ_REP), th("server"), queue(max_size) {
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
    socket.bind("tcp://*:" + std::to_string(port));
}

folly::NotificationQueue<Message> *Server_network_module::get_queue() {
    return &queue;
}

void Server_network_module::messageAvailable(Message &&msg) noexcept {
    if (msg.type() == Message::MessageType::AudioResult) {  // send to one
                                                            // client from mixer
        send_to_one_client(std::move(msg));
    } else {
        // Other MessageType's are not suitable situation for message from
        // mixer to server
        LOG(INFO) << "Received bad message type (to server from mixer) = "
                  << to_string[msg.type()] << "\n";
        assert(false);
    }
}

void Server_network_module::send_to_all_clients_except_one(Message &&msg) {
    LOG(INFO) << "send_to_all: " << to_string[msg.type()] << ", from_id = " << msg.id() << "\n";
    for (auto &[id, v] : messages) {
        if (msg.id() != id) {
            v.push_back(msg);
        }
    }
}

void Server_network_module::send_to_one_client(Message &&msg) {
    send(socket, std::move(msg));
}

Server_network_module::~Server_network_module() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}