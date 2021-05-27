#include "include/Mixer/mixer.h"

std::vector<AudioFile<float>> mixer::split(AudioFile<float> file, double dur) {
    std::vector<AudioFile<float>> splitted;
    splitted.resize(ceil(file.getLengthInSeconds() / dur) + 1);
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

AudioFile<float> mixer::join(const std::vector<AudioFile<float>> &separate) {
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

std::vector<mixer::Message> mixer::try_to_mix(
    std::vector<mixer::Message> &vec) {
    std::vector<mixer::Message> result = vec;
    for (auto &r : result) {
        for (auto v : vec) {
            if (v.id == r.id) {
                r.data.addAudioBuffer(v.data.samples, -1);
            } else {
                r.data.addAudioBuffer(v.data.samples,
                                      mixer::count_coef(r.x, r.y, v.x, v.y));
            }
        }
    }
    return result;
}

mixer::Mixer::Mixer() {
    std::ifstream config("config.txt");
    config >> normal_delay >> number_id;

    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });

    M.resize(number_id);
    sample.samples.resize(1);
    sample.samples[0].resize(2, 0);
}

void mixer::Mixer::messageAvailable(Message &&msg) noexcept {
    M[msg.id].insert(msg);
}

void mixer::Mixer::putMessage(const Message &msg) {
    queue.putMessage(msg);
}

void mixer::Mixer::add_id(int new_ids) {
    if (new_ids <= number_id) {
        return;
    }
    M.resize(new_ids);
    number_id = new_ids;
}

std::vector<mixer::Message> mixer::Mixer::mix() {
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

int main() {
    return 0;
}