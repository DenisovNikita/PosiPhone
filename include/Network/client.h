#pragma once

#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>
#include <iostream>
#include <sstream>
#include <thread>
#include <zmq.hpp>
#include "message.h"
#include "model_fwd.h"
#include "server_fwd.h"

using std::cout, std::cin;
using std::string;
using std::stringstream;

class Client : public folly::NotificationQueue<Message>::Consumer {
private:
    Model *model;
    Server *server;
    zmq::context_t context;
    zmq::socket_t socket;
    folly::ScopedEventBaseThread th;
    const uint32_t max_size = 10'000;
    folly::NotificationQueue<Message> queue;

public:
    explicit Client(Model *model, Server *s);  // creates a client
    int connect_to_server();                   // returns error_code
    void messageAvailable(Message &&msg) noexcept override;
    bool is_ok_connection();

    folly::NotificationQueue<Message> *get_queue();

    ~Client() override;
};