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

namespace PosiPhone {
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
      recorder(setWavFormat(QAudioDeviceInfo::defaultInputDevice())) {
    buffer.open(QBuffer::WriteOnly);
    recorder.start(&buffer);
    runner.add("Recorder", [this, model]() {
        if (isChecked()) {
            std::vector<char> tmp;
            tmp.assign(buffer.buffer().data(),
                       buffer.buffer().data() + buffer.buffer().size());
            model->send_message(Message::create<Message::AudioSource>(
                model->get_id(), model->get_x(), model->get_y(),
                std::make_shared<std::vector<char>>(tmp)));
        }
        buffer.buffer().resize(0);
        buffer.buffer().clear();
        buffer.reset();
        return std::chrono::milliseconds(50);
    });
}

Recorder::~Recorder() {
    runner.stop();
    buffer.close();
}

Player::Player(Model *model, QWidget *parent)
    : Audio("sound", model, parent),
      player(setWavFormat(QAudioDeviceInfo::defaultOutputDevice())) {
    buffer.open(QBuffer::ReadOnly);
    player.start(&buffer);
    runner.add("Player", [this, model]() {
        buffer.buffer().resize(0);
        buffer.buffer().clear();
        buffer.reset();
        Message msg;
        model->read_audio_message(msg);
        if (isChecked() && msg.data()) {
            buffer.buffer().append(msg.data()->data(), msg.data()->size());
        }
        return std::chrono::milliseconds(50);
    });
}

Player::~Player() {
    runner.stop();
    buffer.close();
}

}  // namespace PosiPhone
