#include "message.h"
#include "network_utils.h"
#include "server.h"

namespace {
int counter = 0;
int get_new_id() {
    return ++counter;
}

const int TIMEOUT_MILLISECONDS = 2000;
const int TIME_SLEEP = 100;

int cur_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

}  // namespace

// пинать миксер и скармливать очередь из сервера

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    Server server;
    int start = cur_time();
    std::thread thread_for_clients_messages([&]() {
        while (true) {
            Message msg = receive(server.socket);
            if (msg.id() != 0) {
                int prev = server.last_time_by_id[msg.id()];
                server.last_time_for_all.erase({prev, msg.id()});
                int cur_int = cur_time() - start;
                server.last_time_for_all.insert({cur_int, msg.id()});
                server.last_time_by_id[msg.id()] = cur_int;
            }

            if (msg.type() != Message::MessageType::Check || msg.x() != 2 ||
                msg.y() != 2) {
                LOG(INFO) << "server received from client: "
                          << to_string[msg.type()] << "\n";
            }
            if (msg.type() == Message::MessageType::Check) {
                if (msg.x() == 0 && msg.y() == 0) {
                    Message new_msg =
                        Message::create<Message::MessageType::Check>(msg.id(),
                                                                     1, 0);
                    server.send_to_one_client(std::move(new_msg));
                } else if (msg.x() == 2 && msg.y() == 2) {
                    if (server.messages[msg.id()].empty()) {
                        server.send_to_one_client(
                            Message::create<Message::MessageType::Empty>());
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
                    server.name_by_id[new_id] = msg.name();
                    Message new_msg =
                        Message::create<Message::MessageType::Create>(
                            new_id, msg.name(), msg.x(), msg.y());
                    LOG(INFO) << "new_msg.id() = " << new_msg.id() << "\n";
                    server.send_to_all_clients_except_one(std::move(new_msg));
                    for (auto [id, name] : server.name_by_id) {
                        server.messages[new_id].push_back(
                            Message::create<Message::MessageType::Create>(
                                id, name, server.crds[id].first,
                                server.crds[id].second));
                    }
                }
                Message info_msg =
                    Message::create<Message::MessageType::Connect>(
                        new_id, msg.name(), msg.x(), msg.y());
                server.send_to_one_client(std::move(info_msg));
            } else if (msg.type() == Message::MessageType::AudioSource) {
                // TODO: send msg to mixer
                assert(false);
            } else if (msg.type() ==
                       Message::MessageType::Move) {  // send all, except
                                                      // author
                server.crds[msg.id()] = {msg.x(), msg.y()};
                server.send_to_one_client(Message());
                server.send_to_all_clients_except_one(std::move(msg));
            } else {
                // Other MessageType's are not suitable situation for message
                // from client to server
                LOG(INFO)
                    << "Received bad message type (to server from client) = "
                    << to_string[msg.type()] << "\n";
                assert(false);
            }
        }
    });

    std::thread thread_for_detecting_dead_clients([&]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(TIME_SLEEP));
            int curr_int = cur_time() - start;
            std::set<std::int64_t> removing;
            for (auto [tim, id] : server.last_time_for_all) {
                if (curr_int - tim >= TIMEOUT_MILLISECONDS) {
                    removing.insert(id);
                }
            }

            for (auto id : removing) {
                LOG(INFO) << "id = " << id << " will be removed\n";
                Message new_msg =
                    Message::create<Message::MessageType::Destroy>(id);
                server.send_to_all_clients_except_one(std::move(new_msg));
                server.usernames.erase(server.name_by_id[id]);
                server.crds.erase(id);
                server.name_by_id.erase(id);
                server.messages.erase(id);
                server.last_time_for_all.erase(
                    {server.last_time_by_id[id], id});
                server.last_time_by_id.erase(id);
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
    thread_for_clients_messages.detach();
    thread_for_detecting_dead_clients.detach();
    exit(0);
}
