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
    : QObject(parent), proceed_audio(false), model(model), buffer(&array) {
}

void BaseAudio::button_clicked() {
    proceed_audio ^= true;
}

Recorder::Recorder(Model *model, QObject *parent)
    : BaseAudio(model, parent),
      recorder(setWavFormat(QAudioDeviceInfo::defaultInputDevice())) {
}

void Recorder::start() {
    QMetaObject::invokeMethod(qApp, [this]() {
        buffer.open(QBuffer::WriteOnly);
        recorder.start(&buffer);
        runner.add("Recorder", [this]() {
            LOG(INFO) << "recorder\n";
            if (proceed_audio) {
                model->send_audio_data(std::make_shared<std::vector<char>>(
                    array.begin(), array.end()));
            }
            buffer.seek(0);
            return std::chrono::milliseconds(50);
        });
    });
}

void Recorder::stop() {
    QMetaObject::invokeMethod(qApp, [this]() {
        runner.stop();
        recorder.stop();
        buffer.close();
    });
}

Player::Player(Model *model, QObject *parent)
    : BaseAudio(model, parent),
      player(setWavFormat(QAudioDeviceInfo::defaultOutputDevice())) {
}

void Player::start() {
    QMetaObject::invokeMethod(qApp, [this]() {
        buffer.open(QBuffer::ReadOnly);
        player.start(&buffer);
        runner.add("Player", [this]() {
            LOG(INFO) << "player\n";
            auto ptr = model->receive_audio_data();
            if (proceed_audio && ptr) {
                array = QByteArray(ptr->data(), ptr->size());
            } else {
                array = QByteArray();
            }
            buffer.seek(0);
            return std::chrono::milliseconds(50);
        });
    });
}

void Player::stop() {
    QMetaObject::invokeMethod(qApp, [this]() {
        runner.stop();
        player.stop();
        buffer.close();
    });
}

RecorderRunner::RecorderRunner(Model *model, QObject *parent)
    : QObject(parent), recorder(new Recorder(model)), thread(new QThread) {
    recorder->moveToThread(thread);
    connect(thread, &QThread::started, [&]() { recorder->start(); });
    connect(this, &RecorderRunner::finished, [&]() { recorder->stop(); });
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
    connect(thread, &QThread::started, [&]() { player->start(); });
    connect(this, &PlayerRunner::finished, [&]() { player->stop(); });
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
