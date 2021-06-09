#include "client.h"
#include <glog/logging.h>
#include "model.h"
#include "network_utils.h"

namespace {
const auto server_name =
    "tcp://188.119.67.234:" + std::to_string(PosiPhone::port);

int set_connection(zmq::socket_t &socket) {
    socket.connect(server_name);
    return 0;
}

}  // namespace

namespace PosiPhone {
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

        std::int64_t my_id = model->get_id();
        assert(my_id != 0);
        while (true) {
            Message pullRequest =
                Message::create<Message::Request_new_info>(my_id);
            send(local_socket, std::move(pullRequest));
            auto result = receive(local_socket);
            if (result.type() != Message::Empty) {
                //                LOG(INFO) << "client received from
                //                another_client: "
                //                          << to_string[result.type()] <<
                //                          std::endl;
            }
            if (result.type() == Message::AudioResult) {
                model->send_audio_message(std::move(result));
            } else if (result.type() == Message::Create ||
                       result.type() == Message::Move ||
                       result.type() == Message::Destroy) {
                model->send_message(std::move(result));
            } else if (result.type() == Message::Empty) {
                continue;
            } else {
                LOG(WARNING) << "Unknown query\n";
            }
        }
    });
    LOG(INFO) << "client created" << std::endl;
}

Message Client::send_to_server(Message &&msg) {
    auto res = send_and_receive(socket, std::move(msg));
    if (res.type() != Message::Empty) {
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
    if (msg.type() == Message::Connect || msg.type() == Message::AudioSource ||
        msg.type() == Message::Move || msg.type() == Message::Destroy) {
        auto res = send_to_server(std::move(msg));
        if (res.type() != Message::Empty) {
            LOG(INFO) << "client received from model: " << to_string[res.type()]
                      << std::endl;
        }
        if (res.type() == Message::Connect) {
            LOG(INFO) << "new_id = " << res.id() << std::endl;
            model->send_message(std::move(res));
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
    Message request =
        Message::create<Message::Check_connection>(model->get_id());
    Message response = send_and_receive(socket, std::move(request));
    return response.type() == Message::Check_connection;
}

void Client::send_message(Message &&msg) {
    try {
        queue.tryPutMessage(std::move(msg));
    } catch (const std::exception &e) {
        LOG(ERROR) << e.what() << '\n';
    }
}

Client::~Client() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
    network_thread.detach();
}

}  // namespace PosiPhone
