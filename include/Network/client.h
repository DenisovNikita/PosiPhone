#pragma once

#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <zmq.hpp>
#include "message.h"
#include "model_fwd.h"

class Client : public folly::NotificationQueue<Message>::Consumer {
private:
    Model *model;
    zmq::context_t context;
    zmq::socket_t socket;
    folly::ScopedEventBaseThread th;
    static const uint32_t max_size = 10'000;
    folly::NotificationQueue<Message> queue;
    std::thread network_thread;
    std::mutex m;

public:
    explicit Client(Model *model);  // creates a client
    int connect_to_server();        // returns error_code
    void messageAvailable(Message &&msg) noexcept override;
    bool is_ok_connection();
    Message send_to_server(Message &&msg);

    folly::NotificationQueue<Message> *get_queue();

    ~Client() override;
};
