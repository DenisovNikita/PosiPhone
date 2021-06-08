#include "audio.h"
#include "model.h"

namespace {
QAudioFormat setWavFormat(const QAudioDeviceInfo &dev_info) {
    static QAudioFormat format;
    if (format.sampleRate() == -1) {
        format.setSampleRate(44100);
        format.setChannelCount(1);
        format.setSampleSize(32);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::Float);
    }

    if (!dev_info.isFormatSupported(format)) {
        throw std::runtime_error("WAV format not supported, cannot play audio");
    }

    return format;
}
}  // namespace

namespace PosiPhone {
BaseAudio::BaseAudio(Model *model, QObject *parent)
    : QObject(parent), proceed_audio(false), model(model) {
    buffer.open(QBuffer::ReadWrite);
}

void BaseAudio::button_clicked() {
    proceed_audio ^= true;
}

Recorder::Recorder(Model *model, QObject *parent)
    : BaseAudio(model, parent),
      recorder(setWavFormat(QAudioDeviceInfo::defaultInputDevice())) {
    connect(&recorder, &QAudioInput::notify, this, &Recorder::send_audio);
    recorder.setNotifyInterval(50);
}

void Recorder::send_audio() {
    if (proceed_audio) {
        model->send_audio_data(buffer.read_and_clear());
    } else {
        buffer.clear();
    }
}

void Recorder::start() {
    recorder.start(&buffer);
}

void Recorder::stop() {
    recorder.stop();
}

Player::Player(Model *model, QObject *parent)
    : BaseAudio(model, parent),
      player(setWavFormat(QAudioDeviceInfo::defaultOutputDevice())) {
    connect(&player, &QAudioOutput::notify, this, &Player::receive_audio);
    player.setNotifyInterval(50);
}

void Player::start() {
    player.start(&buffer);
}

void Player::stop() {
    player.stop();
}

void Player::receive_audio() {
    auto data = model->receive_audio_data();
    if (proceed_audio) {
        buffer.clear_and_write(data);
    } else {
        buffer.clear();
    }
}

RecorderRunner::RecorderRunner(Model *model, QObject *parent)
    : QObject(parent), recorder(new Recorder(model)), thread(new QThread) {
    recorder->moveToThread(thread);
    connect(thread, &QThread::started, [&]() {
        QMetaObject::invokeMethod(this, [&]() { recorder->start(); });
    });
    connect(this, &RecorderRunner::finished, [&]() {
        QMetaObject::invokeMethod(this, [&]() { recorder->stop(); });
    });
    connect(thread, &QThread::finished, recorder, &Recorder::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
}

void RecorderRunner::start() {
    thread->start();
}

BaseAudio *RecorderRunner::get() const {
    return recorder;
}

RecorderRunner::~RecorderRunner() noexcept {
    emit finished();
    thread->quit();
    thread->wait();
}

PlayerRunner::PlayerRunner(Model *model, QObject *parent)
    : QObject(parent), player(new Player(model)), thread(new QThread) {
    player->moveToThread(thread);
    connect(thread, &QThread::started, [&]() {
        QMetaObject::invokeMethod(this, [&]() { player->start(); });
    });
    connect(this, &PlayerRunner::finished, [&]() {
        QMetaObject::invokeMethod(this, [&]() { player->stop(); });
    });
    connect(thread, &QThread::finished, player, &Player::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
}

void PlayerRunner::start() {
    thread->start();
}

BaseAudio *PlayerRunner::get() const {
    return player;
}

PlayerRunner::~PlayerRunner() noexcept {
    emit finished();
    thread->quit();
    thread->wait();
}

}  // namespace PosiPhone
