#ifndef MIXER_MIXER_H
#define MIXER_MIXER_H

#include <folly/io/async/NotificationQueue.h>
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
#include "AudioFile/AudioFile.h"

#define NORMAL_DELAY 50
#define NUMBER_OF_ID 5

namespace mixer {

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

std::vector<Message> try_to_mix(std::vector<Message> &vec) {
    std::vector<Message> result = vec;
    for (auto &r : result) {
        for (auto v : vec) {
            if (v.id == r.id) {
                r.data.addAudioBuffer(v.data.samples, -1);
            } else {
                r.data.addAudioBuffer(v.data.samples,
                                      count_coef(r.x, r.y, v.x, v.y));
            }
        }
    }
    return result;
}

class QueueConsumer : public folly::NotificationQueue<Message>::Consumer {
public:
    QueueConsumer() = default;

    void messageAvailable(Message &&value) noexcept override {
        messages.push_back(value);
        if (fn) {
            fn(value);
        }
    }

    std::function<void(Message)> fn;
    std::deque<Message> messages;
};

struct Mixer {
    std::function<bool(const Message &, const Message &)> time_compare =
        [](const auto &a, const auto &b) { return a.time < b.time; };
    std::multiset<Message, decltype(time_compare)> M[NUMBER_OF_ID];
    AudioFile<float> sample;
    folly::EventBase eventBase;
    QueueConsumer consumer;
    folly::NotificationQueue<Message> queue;

    Mixer() {
        consumer.fn = [&](const Message &msg) {
            if (msg.id == -1) {
                consumer.stopConsuming();
            }
        };

        sample.setAudioBufferSize(1, 0);
    }

    void putMessage(Message msg) {
        queue.putMessage(msg);
    }

    std::vector<Message> mix() {
        queue.putMessage(Message{-1, -1, -1, -1, 0, sample});
        consumer.startConsuming(&eventBase, &queue);
        eventBase.loop();
        while (!consumer.messages.empty()) {
            if (consumer.messages.back().id != -1) {
                M[consumer.messages.back().id].insert(consumer.messages.back());
            }
            consumer.messages.pop_back();
        }
        long long ticker = cur_time();
        std::vector<Message> input;
        for (auto &m : M) {
            while (!m.empty() && m.begin()->time < ticker - 100) {
                m.erase(m.begin());
            }
            if (!m.empty() && m.begin()->time >= ticker - 100 &&
                m.begin()->time < ticker - 50) {
                input.push_back(*m.begin());
            }
        }
        if (!input.empty()) {
            return try_to_mix(input);
        } else {
            return std::vector<Message>();
        }
    }
};
}  // namespace mixer

#endif  // MIXER_MIXER_H
