#pragma once

#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <algorithm>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <zmq.hpp>
#include "client_fwd.h"
#include "message.h"

using std::cout, std::cin;
using std::string;
using std::stringstream;
using std::vector;

class Server : public folly::NotificationQueue<Message>::Consumer {
    std::unordered_map<std::int64_t, std::unique_ptr<Client>> clients;
    std::unordered_set<std::string> usernames;
    std::unordered_map<std::int64_t, std::pair<double, double>> crds;
    zmq::context_t context;
    zmq::socket_t socket;
    folly::ScopedEventBaseThread th;
    const uint32_t max_size = 10'000;
    folly::NotificationQueue<Message> queue;
    //    Mixer *mixer;  // TODO: support mixer integrate

public:
    Server();

    folly::NotificationQueue<Message> *get_queue();

    void messageAvailable(Message &&msg) noexcept override;

    ~Server() override;
};