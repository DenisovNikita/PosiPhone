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
#include "mixer/AudioFile/AudioFile.h"

#define NORMAL_DELAY 50
#define NUMBER_OF_ID 5

namespace mixer {

std::vector<AudioFile<float>> split(AudioFile<float> file, double dur) {
    std::vector<AudioFile<float>> splitted;
    splitted.resize(ceil(file.getLengthInSeconds() / dur));
    for (int i = 0; i < splitted.size(); i++) {
        splitted[i].setAudioBufferSize(file.getNumChannels(),
                                       ceil(dur * file.getSampleRate()));
        for (int k = 0; k < file.getNumChannels(); k++) {
            for (int j = 0; j < ceil(dur * file.getSampleRate()); j++) {
                splitted[i].samples[k][j] +=
                    file.samples[k][j + i * ceil(dur * file.getSampleRate())];
            }
        }
    }
    return splitted;
}

AudioFile<float> join(const std::vector<AudioFile<float>> &v) {
    AudioFile<float> joint;
    int channels = 0, len = 0, numSamples = 0;
    for (const auto &c : v) {
        channels = std::max(channels, c.getNumChannels());
        len += c.samples[0].size();
        numSamples += c.getNumSamplesPerChannel();
    }
    joint.samples.resize(channels);
    for (int i = 0; i < channels; i++) {
        joint.samples[i].resize(len);
    }
    len = 0;
    for (const auto &c : v) {
        for (int k = 0; k < c.getNumChannels(); k++) {
            for (int i = 0; i < c.samples[k].size(); i++) {
                joint.samples[k][i + len] = c.samples[k][i];
            }
        }
        len += c.samples[0].size();
    }
    return joint;
}

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

struct time_compare {
    bool operator()(const Message &a, const Message &b) const {
        return (a.time < b.time);
    };
};

struct Mixer {
    std::vector<std::multiset<Message, time_compare>> M;
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
        M.resize(NUMBER_OF_ID);
        sample.samples.resize(1);
        sample.samples[0].resize(2, 0);
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
            while (!m.empty() && m.begin()->time < ticker - NORMAL_DELAY * 2) {
                m.erase(m.begin());
            }
            if (!m.empty() && m.begin()->time >= ticker - 100 &&
                m.begin()->time < ticker - 50) {
                input.push_back(*m.begin());
            }
        }

        return try_to_mix(input);
    }
};
}  // namespace mixer

#endif  // MIXER_MIXER_H
