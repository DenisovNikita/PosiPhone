#include "server_network_module.h"
#include "client.h"
#include "network_utils.h"

namespace PosiPhone {
Server_network_module::Server_network_module()
    : context(IO_THREADS_),
      socket(context, ZMQ_REP),
      th("server"),
      queue(max_size) {
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
    socket.bind("tcp://*:" + std::to_string(port));
}

void Server_network_module::messageAvailable(Message &&msg) noexcept {
    if (msg.type() == Message::MessageType::AudioResult) {  // send to one
                                                            // client from mixer
        ClientServerTimeStamp new_stamp = msg.stamp();
        new_stamp.server_time_received_from_mixer = cur_time();
        msg.set_stamp(new_stamp);
        clients_data.messages[msg.id()].push_back(std::move(msg));
    } else {
        // Other MessageType's are not suitable situation for message from
        // mixer to server
        msg.print("mixer -> server: bad message!");
        assert(false);
    }
}

void Server_network_module::send_to_all_clients_except_one(Message &&msg) {
    LOG(INFO) << "send_to_all: " << to_string[msg.type()]
              << ", from_id = " << msg.id() << "\n";
    for (auto &[id, v] : clients_data.messages) {
        if (msg.id() != id) {
            ClientServerTimeStamp new_stamp = msg.stamp();
            new_stamp.server_time_sent_to_client = cur_time();
            msg.set_stamp(new_stamp);
            v.push_back(msg);
        }
    }
}

void Server_network_module::send_to_one_client(Message &&msg) {
    ClientServerTimeStamp new_stamp = msg.stamp();
    new_stamp.server_time_sent_to_client = cur_time();
    msg.set_stamp(new_stamp);
    send(socket, std::move(msg));
}

Server_network_module::~Server_network_module() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}

void Server_network_module::Clients_data::update_last_time(std::int64_t id,
                                                           int new_time) {
    int prev = last_time_by_id[id];
    last_time_for_all.erase({prev, id});
    last_time_for_all.insert({new_time, id});
    last_time_by_id[id] = new_time;
}

void Server_network_module::Clients_data::add_new_client(
    std::int64_t id,
    const std::string &name,
    double x,
    double y) {
    usernames.insert(name);
    crds[id] = {x, y};
    name_by_id[id] = name;
}

void Server_network_module::Clients_data::remove_client(std::int64_t id) {
    usernames.erase(name_by_id[id]);
    crds.erase(id);
    name_by_id.erase(id);
    messages.erase(id);
    last_time_for_all.erase({last_time_by_id[id], id});
    last_time_by_id.erase(id);
}

}  // namespace PosiPhone
