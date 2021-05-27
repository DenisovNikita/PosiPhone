#include "include/Mixer/mixer.h"
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

std::vector<AudioFile<float>> mixer::split(AudioFile<float> file, double dur) {
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

AudioFile<float> mixer::join(const std::vector<AudioFile<float>> &v) {
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

int main() {
    mixer::Mixer my_mixer;

    const std::string inputFilePath = std::string(PROJECT_BINARY_DIR);
    AudioFile<float> rick;
    rick.load(
        inputFilePath +
        "/../mixer/sound_samples/rick-astley-never-gonna-give-you-up.wav");
    std::vector<AudioFile<float>> vec = mixer::split(rick, 0.05);
    AudioFile<float> zhak;
    zhak.load(inputFilePath +
              "/../mixer/sound_samples/Zhak-Fresco-pro-bidlo.wav");
    std::vector<AudioFile<float>> vec1 = mixer::split(zhak, 0.05);

    std::vector<AudioFile<float>> example[NUMBER_OF_ID];

    int cnt = 0;
    while (cnt < 20) {
        usleep(NORMAL_DELAY * 1000);
        long long ms = mixer::cur_time();
        my_mixer.putMessage(mixer::Message{
            -50.0 + static_cast<double>(cnt) / 20.0, 1, 1, 1, ms, vec[cnt]});
        my_mixer.putMessage(mixer::Message{1, 1, 2, 1, ms, vec1[cnt]});
        cnt++;
        std::vector<mixer::Message> input = my_mixer.mix();
        if (!input.empty()) {
            for (mixer::Message &M : input) {
                example[M.id].push_back(M.data);
            }
        }
    }

    mixer::join(example[1])
        .save(inputFilePath + "/../mixer/mixed_files/testing.wav");

    return 0;
}
