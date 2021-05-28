#include "client.h"
#include <glog/logging.h>
#include <unordered_map>
#include "model.h"
#include "network_utils.h"

namespace PosiPhone {
namespace {
const auto server_name = "tcp://188.119.67.234:" + std::to_string(port);

int set_connection(zmq::socket_t &socket) {
    socket.connect(server_name);
    return 0;
}

const int TIME_SLEEP = 50;

}  // namespace

Client::Client(Model *m)
    : model(m),
      context(IO_THREADS_),
      socket(context, ZMQ_REQ),
      th("client"),
      queue(max_size) {
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });

    // TODO: connect from model via Client::connect_to_server() in order to
    //  write error messages to users
    assert(set_connection(socket) == 0);

    network_thread = std::thread([&]() {
        zmq::socket_t local_socket(context, ZMQ_REQ);
        local_socket.connect(server_name);

        // Could be done with condition variable
        while (model->get_id() == 0) {
        }

        int my_id = model->get_id();
        assert(my_id != 0);
        while (true) {
            //            std::this_thread::sleep_for(std::chrono::milliseconds(TIME_SLEEP));
            Message pullRequest =
                Message::create<Message::MessageType::Request_new_info>(my_id);
            send(local_socket, std::move(pullRequest));
            auto result = receive(local_socket);
            if (result.type() != Message::MessageType::Empty) {
                LOG(INFO) << "client received from another_client: "
                          << to_string[result.type()] << std::endl;
            }
            if (result.type() == Message::MessageType::AudioResult) {
                model->write_audio_message(std::move(result));
            } else if (result.type() == Message::MessageType::Create ||
                       result.type() == Message::MessageType::Move ||
                       result.type() == Message::MessageType::Destroy) {
                model->get_queue()->putMessage(std::move(result));
            } else if (result.type() == Message::MessageType::Empty) {
                continue;
            } else {
                assert(false);  // must not be other message types
            }
        }
    });
    LOG(INFO) << "client created" << std::endl;
}

Message Client::send_to_server(Message &&msg) {
    auto res = send_and_receive(socket, std::move(msg));
    if (res.type() != Message::MessageType::Empty) {
        LOG(INFO) << "client received reply for server request: "
                  << to_string[res.type()] << std::endl;
    }
    return res;
}

int Client::connect_to_server() {
    return set_connection(socket);
}

void Client::messageAvailable(Message &&msg) noexcept {
    std::unique_lock l(m);
    if (msg.type() == Message::MessageType::Connect ||
        msg.type() == Message::MessageType::AudioSource ||
        msg.type() == Message::MessageType::Move ||
        msg.type() == Message::MessageType::Destroy) {
        auto res = send_to_server(std::move(msg));
        if (res.type() != Message::MessageType::Empty) {
            LOG(INFO) << "client received from model: " << to_string[res.type()]
                      << std::endl;
        }
        if (res.type() == Message::MessageType::Connect) {
            LOG(INFO) << "new_id = " << res.id() << std::endl;
            model->get_queue()->putMessage(std::move(res));
        }
    } else {
        assert(false);  // must not be other MessageType from model to client
    }
}

bool Client::is_ok_connection() {
    std::unique_lock l(m);
    if (model->get_id() == 0) {
        return true;
    }
    Message request = Message::create<Message::MessageType::Check_connection>(
        model->get_id());
    Message response = send_and_receive(socket, std::move(request));
    return response.type() == Message::MessageType::Check_connection;
}

Client::~Client() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
    network_thread.detach();
}

}  // namespace PosiPhone
