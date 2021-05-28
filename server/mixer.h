#ifndef MIXER_MIXER_H
#define MIXER_MIXER_H

//#define _GLIBCXX_DEBUG

/*
♿ ОСТОРОЖНО ♿ ПРОГРАММИРУЕТ ♿ ГРОМОВ ♿ ФЕДОР ♿
*/

#include <folly/experimental/ThreadedRepeatingFunctionRunner.h>
#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "AudioFile.h"
#include "message.h"

namespace utils {

class utils {
public:
    long long cur_time();
    double count_coef(double x1, double y1, double x2, double y2);
};

}  // namespace utils

namespace PosiPhone {

std::vector<AudioFile<float>> split(AudioFile<float> file, double dur);

AudioFile<float> join(const std::vector<AudioFile<float>> &separate);

struct AudioMessage {
    const double x;
    const double y;
    const std::int64_t id;
    const long long time;
    AudioFile<float> data;
    AudioMessage(double x_,
                 double y_,
                 std::int64_t id_,
                 long long time_,
                 AudioFile<float> data_)
        : x(x_), y(y_), id(id_), time(time_), data(std::move(data_)){};
};

std::vector<AudioMessage> try_to_mix(std::vector<AudioMessage> &vec);

struct time_compare {
    bool operator()(const AudioMessage &a, const AudioMessage &b) const {
        return (a.time < b.time);
    };
};

class Mixer final : public folly::NotificationQueue<AudioMessage>::Consumer {
    folly::ScopedEventBaseThread th;
    std::vector<std::multiset<AudioMessage, time_compare>> messages_sorted;
    AudioFile<float> sample;
    folly::NotificationQueue<AudioMessage> queue;
    long long normal_delay = 50, number_id = 10;
    std::vector<AudioMessage> request_answer = {};
    folly::ThreadedRepeatingFunctionRunner runner;
    folly::NotificationQueue<Message> *result;

public:
    Mixer(folly::NotificationQueue<Message> *result_);
    void messageAvailable(AudioMessage &&msg) noexcept override;
    void putMessage(Message &&msg);
    void add_id(int new_ids);
    void send_messages();
    std::vector<AudioMessage> mix();
    ~Mixer() override {
        runner.stop();
        th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
    }
};
}  // namespace PosiPhone

#endif  // MIXER_MIXER_H
