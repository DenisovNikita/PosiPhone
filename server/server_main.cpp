#include "message.h"
#include "network_utils.h"
#include "server.h"

namespace {
int counter = 0;
int get_new_id() {
    return ++counter;
}

}  // namespace

int main() {
    Server server;
    // server work infinitely
    std::thread t([&]() {
        while (true) {
            Message msg = receive(server.socket);
            std::cerr << "server received: " << msg.type() << "\n";
            if (msg.type() == Message::MessageType::Check) {
                if (msg.x() == 0 && msg.y() == 0) {
                    Message new_msg =
                        Message::create<Message::MessageType::Check>(msg.id(),
                                                                     1, 0);
                    server.send_to_one_client(std::move(new_msg));
                } else if (msg.x() == 2 && msg.y() == 2) {
                    if (server.messages[msg.id()].empty()) {
                        server.send_to_one_client(Message());
                    } else {
                        auto response = server.messages[msg.id()].front();
                        server.send_to_one_client(std::move(response));
                        server.messages[msg.id()].pop_front();
                    }
                } else {
                    assert(false);
                }
            } else if (msg.type() ==
                       Message::MessageType::
                           Connect) {  // give client permission to create user
                int new_id = -1;
                if (!server.usernames.count(msg.name())) {
                    new_id = get_new_id();
                    server.usernames.insert(msg.name());
                    server.crds[new_id] = {msg.x(), msg.y()};
                    Message new_msg =
                        Message::create<Message::MessageType::Create>(
                            new_id, msg.name(), msg.x(), msg.y());
                    server.send_to_all_clients_except_one(std::move(new_msg));
                }
                Message info_msg =
                    Message::create<Message::MessageType::Connect>(msg.name());
                server.send_to_one_client(std::move(info_msg));
            } else if (msg.type() == Message::MessageType::AudioSource) {
                // TODO: send msg to mixer
                assert(false);
            } else if (msg.type() == Message::MessageType::Move ||
                       msg.type() ==
                           Message::MessageType::Destroy) {  // send all, except
                                                             // author
                if (msg.type() == Message::MessageType::Move) {
                    server.crds[msg.id()] = {msg.x(), msg.y()};
                } else if (msg.type() == Message::MessageType::Destroy) {
                    server.usernames.erase(msg.name());
                    server.crds.erase(msg.id());
                } else {
                    assert(false);
                }
                server.send_to_one_client(Message());
                server.send_to_all_clients_except_one(std::move(msg));
            } else {
                // Other MessageType's are not suitable situation for message
                // from client to server
                std::cerr
                    << "Received bad message type (to server from client) = |"
                    << msg.type() << "|\n";
                continue;
                assert(false);
            }
        }
    });

    while (true) {
        std::string command;
        std::cin >> command;
        if (command == "exit") {
            break;
        }
    }
    exit(0);
}
