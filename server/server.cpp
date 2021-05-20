#include "server.h"
#include "client.h"
#include "message.h"

namespace {
int counter = 0;
int get_new_id() {
    return ++counter;
}

}  // namespace

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
    if (msg.type() ==
        Message::MessageType::Connect) {  // give client permission to create
                                          // user
        int new_id = -1;
        if (!usernames.count(msg.name())) {
            new_id = get_new_id();
            usernames.insert(msg.name());
            //            TODO: get ptr to client from msg
            //            clients[new_id] = msg.ptr_client;
            crds[new_id] = {msg.x(), msg.y()};
            Message new_msg = Message::create<Message::MessageType::Create>(
                new_id, msg.name(), msg.x(), msg.y());
            for (auto &[id, ptr_client] : clients) {
                ptr_client->get_queue()->putMessage(new_msg);
            }
        }
        // TODO: send user his new_id. How? Server don't know his id yet...
    } else if (msg.type() ==
               Message::MessageType::AudioResult) {  // send to one client from
                                                     // mixer
        clients[msg.id()]->get_queue()->putMessage(std::move(msg));
    } else if (msg.type() == Message::MessageType::AudioSource) {
        // TODO: send msg to mixer
    } else if (msg.type() == Message::MessageType::Move ||
               msg.type() ==
                   Message::MessageType::Destroy) {  // send all, except author
        if (msg.type() == Message::MessageType::Move) {
            crds[msg.id()] = {msg.x(), msg.y()};
        } else if (msg.type() == Message::MessageType::Destroy) {
            clients.erase(msg.id());
            usernames.erase(msg.name());
            crds.erase(msg.id());
        } else {
            assert(false);
        }
        for (auto &[id, ptr_client] : clients) {
            if (msg.id() != id) {
                ptr_client->get_queue()->putMessage(msg);
            }
        }
    } else {
        // There msg.type() can be equal Message::MessageType::Create, but it
        // is not suitable situation for server
        std::cerr << "Received unknown (to server) message type = |"
                  << msg.type() << "|\n";
        assert(false);
    }
}

Server::~Server() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}

int main() {
    Server server;
    // server work infinitely
    while (true) {
    }
}
