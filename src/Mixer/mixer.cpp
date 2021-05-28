#include "mixer.h"

long long utils::utils::cur_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

double utils::utils::count_coef(double x1, double y1, double x2, double y2) {
    return 1.0 / fmax(1.0, sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}

namespace PosiPhone {
std::vector<AudioFile<float>> split(AudioFile<float> file, double dur) {
    std::vector<AudioFile<float>> splitted;
    splitted.resize(floor(file.getLengthInSeconds() / dur));
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

AudioFile<float> join(const std::vector<AudioFile<float>> &separate) {
    AudioFile<float> joint;
    int channels = 0, len = 0, numSamples = 0;
    for (const auto &c : separate) {
        channels = std::max(channels, c.getNumChannels());
        len += c.samples[0].size();
        numSamples += c.getNumSamplesPerChannel();
    }
    joint.samples.resize(channels);
    for (int i = 0; i < channels; i++) {
        joint.samples[i].resize(len);
    }
    len = 0;
    for (const auto &c : separate) {
        for (int k = 0; k < c.getNumChannels(); k++) {
            for (int i = 0; i < c.samples[k].size(); i++) {
                joint.samples[k][i + len] = c.samples[k][i];
            }
        }
        len += c.samples[0].size();
    }
    return joint;
}

std::vector<AudioMessage> try_to_mix(std::vector<AudioMessage> &vec) {
    std::vector<AudioMessage> result = vec;
    for (auto &r : result) {
        for (auto v : vec) {
            if (v.id == r.id) {
                r.data.addAudioBuffer(v.data.samples, -1);
            } else {
                r.data.addAudioBuffer(v.data.samples, utils::utils().count_coef(
                                                          r.x, r.y, v.x, v.y));
            }
        }
    }
    return result;
}

void Mixer::messageAvailable(AudioMessage &&msg) noexcept {
    if (messages_sorted[msg.id].empty()) {
        messages_sorted[msg.id].emplace(msg);
    } else {
        messages_sorted[msg.id].insert(msg);
    }
}

void Mixer::putMessage(Message &&msg) {
    std::vector<char> vec(msg.data(), msg.data() + msg.size());
    auto *ptr = reinterpret_cast<float *>(vec.data());
    int size = vec.size() / sizeof(float);
    std::vector<std::vector<float>> buf(1, std::vector<float>(size));
    for (int i = 0; i < size; ++i) {
        buf[0][i] = *ptr++;
    }
    AudioFile<float> af;
    af.setAudioBuffer(buf);
    queue.putMessage(AudioMessage{msg.x(), msg.y(), msg.id(), msg.time(), af});
}

void Mixer::add_id(int new_ids) {
    if (new_ids <= number_id) {
        return;
    }
    messages_sorted.resize(new_ids);
    number_id = new_ids;
}

void Mixer::send_messages() {
    for (const AudioMessage &af : request_answer) {
        std::vector<char> vec;
        for (int i = 0; i < af.data.getNumSamplesPerChannel(); ++i) {
            for (int j = 0; j < af.data.getNumChannels(); ++j) {
                const char *ptr =
                    reinterpret_cast<const char *>(&af.data.samples[j][i]);
                int size = sizeof(float);
                vec.insert(vec.end(), ptr, ptr + size);
            }
        }
        result->putMessage(Message::create<Message::AudioResult>(
            af.id, vec.data(), vec.size()));
    }
}

std::vector<AudioMessage> Mixer::mix() {
    long long ticker = utils::utils().cur_time();
    std::vector<AudioMessage> input;
    for (auto &m : messages_sorted) {
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

Mixer::Mixer(folly::NotificationQueue<Message> *result_) : result(result_) {
    std::ifstream config("include/Mixer/config.txt");
    config >> normal_delay >> number_id;
    messages_sorted.resize(10);
    sample.samples.resize(1);
    sample.samples[0].resize(2, 0);

    runner.add("Mixer", [this]() {
        request_answer = mix();
        return std::chrono::milliseconds(50);
    });

    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
}

}  // namespace PosiPhone
