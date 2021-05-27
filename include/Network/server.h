#pragma once

#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <glog/logging.h>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <cstdint>
#include <iostream>
#include <set>
#include <zmq.hpp>
#include "client_fwd.h"
#include "message.h"

class Server : public folly::NotificationQueue<Message>::Consumer {
public:
    std::unordered_set<std::string> usernames;
    std::unordered_map<std::int64_t, std::pair<double, double>> crds;
    std::unordered_map<std::int64_t, std::string> name_by_id;
    std::unordered_map<std::int64_t, std::deque<Message>> messages;
    std::set<std::pair<int, std::int64_t>> last_time_for_all;
    std::unordered_map<std::int64_t, int> last_time_by_id;
    zmq::context_t context;
    zmq::socket_t socket;
    folly::ScopedEventBaseThread th;
    const uint32_t max_size = 10'000;
    folly::NotificationQueue<Message> queue;
    //    Mixer *mixer;  // TODO: support mixer integrate

    Server();

    folly::NotificationQueue<Message> *get_queue();

    void messageAvailable(Message &&msg) noexcept override;

    void send_to_all_clients_except_one(Message &&msg);

    void send_to_one_client(Message &&msg);

    ~Server() override;
};
