#include "audio.h"

namespace {
[[maybe_unused]] QByteArray ARR;
QIcon init_icon(const std::string &name, bool b) {
    std::string path = ":/" + name + (b ? "_on.png" : "_off.png");
    return QIcon(path.c_str());
}
QAudioFormat setWavFormat(const QAudioDeviceInfo &dev_info) {
    static QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    if (dev_info.isFormatSupported(format)) {
        return format;
    } else {
        throw std::runtime_error("WAV format not supported, cannot play audio");
    }
}
}  // namespace

Audio::Audio(const std::string &name, QWidget *parent) : QPushButton(parent) {
    setCheckable(true);
    setChecked(true);
    for (int i = 0; i < 2; ++i) {
        icons[i] = init_icon(name, i);
    }
    setFixedSize(56, 48);
    setIcon();
    connect(this, &QPushButton::clicked, this, &Audio::switch_button);
}

void Audio::setIcon() {
    QPushButton::setIcon(icons[isChecked()]);
    setIconSize(QSize(48, 40));
}

void Audio::switch_button() {
    setIcon();
}

Recorder::Recorder(QWidget *parent)
    : Audio("micro", parent),
      recorder(setWavFormat(QAudioDeviceInfo::defaultInputDevice())) {
    buffer.open(QBuffer::WriteOnly);
    recorder.start(&buffer);
//    runner.add("Recorder", [this]() {
//        if (isChecked()) {
//            ARR = buffer.buffer(); // TODO
//            buffer.buffer().clear();
//        }
//        return std::chrono::milliseconds(5000);
//    });
}

Recorder::~Recorder() {
    runner.stop();
}

QByteArray Recorder::record() {
    auto res = buffer.buffer();
    buffer.buffer().clear();
    return res;
}

Player::Player(QWidget *parent)
    : Audio("sound", parent),
      player(setWavFormat(QAudioDeviceInfo::defaultOutputDevice())) {
    buffer.open(QBuffer::ReadOnly);
    player.start(&buffer);
//    runner.add("Player", [this]() {
//        buffer.buffer() = ARR; // TODO
//        return std::chrono::milliseconds(5000);
//    });
}

Player::~Player() {
    runner.stop();
}

void Player::play(const QByteArray &array) {
    buffer.buffer() = array;
}
