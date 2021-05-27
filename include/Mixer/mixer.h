#ifndef MIXER_MIXER_H
#define MIXER_MIXER_H

#define _GLIBCXX_DEBUG

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

namespace mixer {

std::vector<AudioFile<float>> split(AudioFile<float> file, double dur);

AudioFile<float> join(const std::vector<AudioFile<float>> &separate);

long long cur_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

double count_coef(double x1, double y1, double x2, double y2) {
    return 1.0 / fmax(1.0, sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}

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
    folly::ScopedEventBaseThread th;
    std::vector<std::multiset<Message, time_compare>> M;
    AudioFile<float> sample;
    folly::NotificationQueue<Message> queue;
    long long normal_delay = 0, number_id = 0;

public:
    Mixer() {
        std::ifstream config("config.txt");
        config >> normal_delay >> number_id;

        auto *eventBase = th.getEventBase();
        eventBase->runInEventBaseThread(
            [eventBase, this]() { startConsuming(eventBase, &queue); });

        M.resize(number_id);
        sample.samples.resize(1);
        sample.samples[0].resize(2, 0);
    }

    void messageAvailable(Message &&msg) noexcept override {
        M[msg.id].insert(msg);
    }

    void putMessage(Message msg) {
        queue.putMessage(msg);
    }

    void add_id(int new_ids) {
        if (new_ids <= number_id) {
            return;
        }
        M.resize(new_ids);
        number_id = new_ids;
    }

    std::vector<Message> mix() {
        long long ticker = cur_time();
        std::vector<Message> input;
        for (auto &m : M) {
            while (!m.empty() && m.begin()->time < ticker - normal_delay * 2) {
                m.erase(m.begin());
            }
            if (!m.empty() && m.begin()->time >= ticker - normal_delay * 2 &&
                m.begin()->time < ticker - normal_delay) {
                input.push_back(*m.begin());
            }
        }

        return try_to_mix(input);
    }
};
}  // namespace mixer

#endif  // MIXER_MIXER_H
