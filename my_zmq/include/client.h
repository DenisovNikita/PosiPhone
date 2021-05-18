#pragma once

#include "model_fwd.h"

class Client :
        public folly::NotificationQueue<Message>::Consumer {
    Model *model;
    zmq::context_t context;
    zmq::socket_t socket;

public:
    explicit Client(Model *model);  // creates a client
    int connect_to_server();  // returns error_code
//    void start_networking();  // starts send / receive messages
    void messageAvailable(Message &&msg) noexcept override;
    bool is_ok_connection();

    ~Client() override;
};