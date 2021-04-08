#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include "AudioFile/AudioFile.h"
#include <cassert>
#include <sstream>
#include <utility>
#include <vector>
#include <filesystem>

struct Element {
private:
    int x, y, id = -1, room_id;
    AudioFile<float> data;
public:
    Element(int x_, int y_, int id_, int room_id_, AudioFile<float> data_) : x(x_), y(y_), id(id_), room_id(room_id_),
                                                                             data(std::move(data_)) {};

    std::pair<int, int> get_position() {
        assert(id != -1);
        return std::make_pair(x, y);
    }

    [[nodiscard]] int get_id() const {
        assert(id != -1);
        return id;
    }

    [[nodiscard]] int get_room_id() const {
        assert(id != -1);
        return room_id;
    }

    AudioFile<float> get_data() {
        assert(id != -1);
        return data;
    }

};

//creates file + a path to file with name "path" + num + ".wav"

std::string create_path(const std::string &path, const int num) {
    std::stringstream ss;
    ss << num;
    std::string str = ss.str();
    std::ofstream outfile(path + str + ".wav");
    return path + str + ".wav";
}

//splits file into pieces with chosen duration.

std::vector<AudioFile<float> > split(AudioFile<float> file, double dur) {

    std::vector<AudioFile<float> > splitted;
    splitted.resize(ceil(file.getLengthInSeconds() / dur));
    for (int i = 0; i < file.getLengthInSeconds() / double(ceil(dur)); i++) {

        splitted[i].setAudioBufferSize(file.getNumChannels(), ceil(dur * file.getSampleRate()));

        for (int k = 0; k < file.getNumChannels(); k++) {
            for (int j = 0; j < ceil(dur * file.getSampleRate()); j++) {
                splitted[i].samples[k][j] += file.samples[k][j + i * ceil(dur * file.getSampleRate())];
                //std::cout << j + i * ceil(dur * file.getSampleRate()) << std::endl;
            }
        }
    }
    return splitted;
}

//takes files from input, distances from dist, mixes them and output

void mix(const std::string &sound_sources, const std::string &sound_path, const std::string &distance,
         const std::string &mixed_files) {
    std::string file_names;
    int n, m, num_channels = 0, num_sample_per_channel = 0;

    std::ifstream file(sound_sources);
    file >> n;

    int Dist[n][n];
    std::string s;
    AudioFile<float> in[n];
    AudioFile<float> out[n];


    for (int i = 0; i < n; i++) {
        file >> s;
        in[i].load(sound_path + s);
        if (in[i].getNumChannels() > num_channels) {
            num_channels = in[i].getNumChannels();
        }
        if (in[i].getNumSamplesPerChannel() > num_sample_per_channel) {
            num_sample_per_channel = in[i].getNumSamplesPerChannel();
        }
    }

    std::ifstream dist(distance);
    dist >> m;
    assert(n == m);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            dist >> Dist[i][j];
        }
    }

    for (int i = 0; i < n; i++) {
        out[i].setAudioBufferSize(num_channels, num_sample_per_channel);
        for (int j = 0; j < n; j++) {
            out[i].addAudioBuffer(in[j].samples, Dist[i][j]);
        }
        std::string path = create_path(mixed_files, i);
        out[i].save(path);
    }

}

//joins all files from the vector to the single file

AudioFile<float> join(const std::vector<AudioFile<float>> &v) {
    AudioFile<float> joint;
    int channels = 0, len = 0;
    for (const auto &c:v) {
        channels = std::max(channels, c.getNumChannels());
        len += c.getSampleRate() * c.getNumSamplesPerChannel();
        joint.setAudioBufferSize(channels, len);
        for (int k = 0; k < channels; k++) {
            for (int i = len; i < (len + c.getSampleRate() * c.getNumSamplesPerChannel()); i++) {
                joint.samples[k][i] = c.samples[k][i - len];
            }
        }
    }
    return joint;
}

int main() {
}
