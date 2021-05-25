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
#include <chrono>
#include <folly/io/async/NotificationQueue.h>
/* This code will query a ntp server for the local time and display

 * it.  it is intended to show how to use a NTP server as a time
 * source for a simple network connected device.
 * This is the C version.  The orignal was in Perl
 *
 * For better clock management see the offical NTP info at:
 * http://www.eecis.udel.edu/~ntp/
 *
 * written by Tim Hogard (thogard@abnormal.com)
 * Thu Sep 26 13:35:41 EAST 2002
 * Converted to C Fri Feb 21 21:42:49 EAST 2003
 * this code is in the public domain.
 * it can be found here http://www.abnormal.com/~thogard/ntp/
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define NORMAL_DELAY 50
#define NUMBER_OF_ID 5


struct Message {
    const double x;
    const double y;
    const int id;
    const int room_id;
    const long long time;
    AudioFile<float> data;

    Message(double x_, double y_, int id_, int room_id, long long time_, AudioFile<float> data_)
            : x(x_), y(y_), id(id_), room_id(room_id), time(time_), data(std::move(data_)) {};
};

std::string add_number_to_name(const std::string &name, const int num){
    std::stringstream ss;
    ss << num;
    std::string str = ss.str();
    //std::ofstream outfile(name + str + ".wav");
    return name + str;
}

//creates file + a path to file with name "path" + num + ".wav"

std::string create_path(const std::string &path, const int num) {
    std::stringstream ss;
    ss << num;
    std::string str = ss.str();
    //std::ofstream outfile(path + str + ".wav");
    return path + str + ".wav";
}

//splits file into pieces with chosen duration.

std::vector<AudioFile<float> > split(AudioFile<float> file, double dur) {

    std::vector<AudioFile<float> > splitted;
    splitted.resize(ceil(file.getLengthInSeconds() / dur));
    //std::cout << splitted.size() << std::endl;
    for (int i = 0; i < splitted.size(); i++) {

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
    int channels = 0, len = 0, numSamples = 0;
    for (const auto &c:v) {
        channels = std::max(channels, c.getNumChannels());
        len += c.samples[0].size();
        numSamples += c.getNumSamplesPerChannel();
    }
    joint.samples.resize(channels);
    for(int i = 0; i<channels; i++){
        joint.samples[i].resize(len);
    }
    //std::cout << channels << ' ' << len << '\n';
    len = 0;
    for (const auto &c:v) {
        for (int k = 0; k < c.getNumChannels(); k++) {
            for (int i = 0; i < c.samples[k].size(); i++) {
                joint.samples[k][i + len] = c.samples[k][i];
            }
        }
        len+=c.samples[0].size();
    }
    return joint;
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

long long cur_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

double count_coef(double x1, double y1, double x2, double y2){
    return pow(1.0/fmax(1.0, sqrt((x1-x2)*(x1-x2) + (y1 - y2) * (y1 - y2) ) );
}

std::vector<Message> try_to_mix(std::vector<Message>& vec){
    std::vector<Message> result = vec;
    for(auto &r:result){
        std::cout << r.id << ' ' << r.room_id << ' ' << r.data.samples[0].size() << std::endl;
        for(auto v:vec){
            if(v.id == r.id){
                r.data.addAudioBuffer(v.data.samples, -1);
            } else {
                r.data.addAudioBuffer(v.data.samples, count_coef(r.x, r.y, v.x, v.y));
                //std::cout << count_coef(r.x, r.y, v.x, v.y) << ' ';
            }
        }
    }
    std::cout << std::endl;
    return result;
}

int main() {
    auto time_compare = [](const auto &a, const auto &b) { return a.time < b.time; };
    std::multiset<Message, decltype(time_compare)> M[NUMBER_OF_ID];
    AudioFile<float> emptyAudio;
    const std::string inputFilePath = std::string(PROJECT_BINARY_DIR);
    AudioFile<float> rick;
    rick.load(inputFilePath + "/sound_samples/rick-astley-never-gonna-give-you-up.wav");
    std::vector<AudioFile<float>> vec = split(rick, 0.05);
    AudioFile<float> zhak;
    zhak.load(inputFilePath + "/sound_samples/Zhak-Fresco-pro-bidlo.wav");
    std::vector<AudioFile<float>> vec1 = split(zhak, 0.05);
    for(int i = 0; i<vec.size(); i++){
        vec[i].save(create_path(inputFilePath + "/splitted/output", i));
    }
    AudioFile<float> joint11 = join(vec);
    joint11.save(inputFilePath + "/mixed_files/output_kek.wav");

    folly::EventBase eventBase;
    QueueConsumer consumer;
    consumer.fn = [&](const Message &msg) {
        if (msg.id == -1) {
            consumer.stopConsuming();
        }
    };

    std::vector<AudioFile<float>> example[NUMBER_OF_ID];

    folly::NotificationQueue<Message> queue;
    consumer.startConsuming(&eventBase, &queue);
    long long cnt = 0, ticker = cur_time() + 100;
    int counter = 0;
    while (cnt < 50) {
        usleep(NORMAL_DELAY*1000);
        long long ms = cur_time();
        queue.putMessage(Message{-5.0+static_cast<double>(cnt)/20.0, 1, 1, int(cnt), ms, vec[cnt]});
        queue.putMessage(Message{1, 1, 2, int(cnt), ms, vec1[cnt]});
        queue.putMessage(Message{-1, -1, -1, -1, 0, vec1[cnt]});
        cnt++;
        eventBase.loop();
        while (!consumer.messages.empty()) {
            //std::cout << consumer.messages.back().id << std::endl;
            if (consumer.messages.back().id != -1) {
                M[consumer.messages.back().id].insert(consumer.messages.back());
            }
            consumer.messages.pop_back();
        }

        consumer.startConsuming(&eventBase, &queue);
        //std::cout << cur_time() << ' ' << ticker << '\n';
        if(cur_time() >= ticker){
            //std::cout << "ticker in progress\n";
            std::vector<Message> input;
            for(auto & m : M){
                while(!m.empty() && m.begin()->time < ticker - 100){
                    //std::cout << m.begin()->time << std::endl;
                    m.erase(m.begin());
                }
                if(!m.empty() && m.begin()->time >= ticker - 100 && m.begin()->time < ticker - 50){
                    input.push_back(*m.begin());
                }
            }
            if(!input.empty()) {
                std::vector<Message> output = try_to_mix(input);
                for (int i = 0; i < output.size(); i++) {
                    example[i].push_back(output[i].data);
                    std::cout << "pushed to example[" << i << "]\n";
                    //output[i].data.save(create_path(add_number_to_name(inputFilePath + "/splitted", i) + "/output", counter));
                }
            }
            ticker+=50;
            counter++;
        }
    }
    //std::cout << cur_time() << ' ' << M[1].size() << ' ' << M[2].size() << '\n';
    while(counter < 40){
        if(cur_time() >= ticker){
            std::cout << "ticker " << counter << " in progress\n";
            std::vector<Message> input;
            for(auto & m : M){
                while(!m.empty() && m.begin()->time < ticker - 2*NORMAL_DELAY){
                    m.erase(m.begin());
                }
                if(!m.empty() && m.begin()->time >= ticker - 2*NORMAL_DELAY && m.begin()->time < ticker - NORMAL_DELAY){
                    //std::cout << m.begin()->time << std::endl;
                    input.push_back(*m.begin());
                    m.erase(m.begin());
                }
            }
            if(!input.empty()) {
                std::vector<Message> output = try_to_mix(input);
                for (int i = 0; i < output.size(); i++) {
                    example[i].push_back(output[i].data);
                    std::cout << "pushed to example[" << i << "]\n";
                    //output[i].data.save(create_path(add_number_to_name(inputFilePath + "/splitted", i) + "/output", counter));
                }
            }
            ticker+=50;
            counter++;
        }
    }
    //std::cout << cur_time() << std::endl;
    //std::cout << example[0].size() << '\n';
    AudioFile<float> joint0 = join(example[0]);
    AudioFile<float> joint1 = join(example[1]);
    joint0.save(inputFilePath + "/mixed_files/output_rick.wav");
    joint1.save(inputFilePath + "/mixed_files/output_zhak.wav");
    return 0;
}
