#include "audio.h"
#include "model.h"

namespace {
QIcon init_icon(const std::string &name, bool b) {
    std::string path = ":/" + name + (b ? "_on.png" : "_off.png");
    return QIcon(path.c_str());
}
QAudioFormat setWavFormat(const QAudioDeviceInfo &dev_info) {
    static QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(32);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::Float);

    if (dev_info.isFormatSupported(format)) {
        return format;
    } else {
        throw std::runtime_error("WAV format not supported, cannot play audio");
    }
}
}  // namespace

Audio::Audio(const std::string &name, Model *model, QWidget *parent)
    : QPushButton(parent), model(model) {
    setCheckable(true);
    setChecked(false);
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

Recorder::Recorder(Model *model, QWidget *parent)
    : Audio("micro", model, parent),
      recorder(setWavFormat(QAudioDeviceInfo::defaultInputDevice())),
      array(buffer.buffer()) {
    buffer.open(QBuffer::WriteOnly);
    recorder.start(&buffer);
    runner.add("Recorder", [this, model]() {
        if (isChecked()) {
            model->write_audio_message(Message::create<Message::AudioSource>(
                model->get_id(), array.data(), array.size()));
            array.clear();
        }
        return std::chrono::milliseconds(50);
    });
}

Recorder::~Recorder() {
    runner.stop();
    buffer.close();
}

Player::Player(Model *model, QWidget *parent)
    : Audio("sound", model, parent),
      player(setWavFormat(QAudioDeviceInfo::defaultOutputDevice())),
      array(buffer.buffer()) {
    buffer.open(QBuffer::ReadOnly);
    player.start(&buffer);
    runner.add("Player", [this, model]() {
        if (isChecked()) {
            Message msg;
            if (model->read_audio_message(msg)) {
                array.clear();
                array.append(msg.data(), msg.size());
            }
        }
        return std::chrono::milliseconds(50);
    });
}

Player::~Player() {
    runner.stop();
    buffer.close();
}
