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
#include "mixer/AudioFile/AudioFile.h"

namespace utils {

class utils {
public:
    long long cur_time();
    double count_coef(double x1, double y1, double x2, double y2);
};

}  // namespace utils

namespace mixer {

std::vector<AudioFile<float>> split(AudioFile<float> file, double dur);

AudioFile<float> join(const std::vector<AudioFile<float>> &separate);

struct Message {
    const double x;
    const double y;
    const int id;
    const int room_id;
    const long long time;
    AudioFile<float> data;
    Message(double x_,
            double y_,
            int id_,
            int room_id,
            long long time_,
            AudioFile<float> data_)
        : x(x_),
          y(y_),
          id(id_),
          room_id(room_id),
          time(time_),
          data(std::move(data_)){};
};

std::vector<Message> try_to_mix(std::vector<Message> &vec);

struct time_compare {
    bool operator()(const Message &a, const Message &b) const {
        return (a.time < b.time);
    };
};

class Mixer : public folly::NotificationQueue<Message>::Consumer {
private:
    // folly::ThreadedRepeatingFunctionRunner runner;
    folly::ScopedEventBaseThread th;
    std::vector<std::multiset<Message, time_compare>> messages_sorted;
    AudioFile<float> sample;
    folly::NotificationQueue<Message> queue;
    long long normal_delay = 50, number_id = 0;
    std::vector<Message> request_answer = {};

public:
    Mixer() {
        std::ifstream config("include/Mixer/config.txt");
        config >> normal_delay >> number_id;
        messages_sorted.resize(10);
        for (auto &m : messages_sorted) {
            m.clear();
        }
        sample.samples.resize(1);
        sample.samples[0].resize(2, 0);

        auto *eventBase = th.getEventBase();
        eventBase->runInEventBaseThread(
            [eventBase, this]() { startConsuming(eventBase, &queue); });
    }
    void messageAvailable(Message &&msg) noexcept override;
    void putMessage(const Message &msg);
    void add_id(int new_ids);
    std::vector<Message> mix();
};
}  // namespace mixer

#endif  // MIXER_MIXER_H
