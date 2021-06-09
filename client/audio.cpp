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
    connect(&recorder, &QAudioInput::notify, this, &Recorder::send_audio);
    recorder.setNotifyInterval(50);
}

void Recorder::send_audio() {
//    QMetaObject::invokeMethod(qApp, [&]() { recorder.suspend(); });
    if (proceed_audio) {
        std::vector<char> tmp;
        tmp.assign(array.data(), array.data() + array.size());
        model->send_audio_data(std::make_shared<std::vector<char>>(tmp));
    }
//    array.clear();
    buffer.seek(0);
    qDebug() << "recorder:" << buffer.pos() << buffer.size() << array.size();
//    QMetaObject::invokeMethod(qApp, [&]() { recorder.resume(); });
}

void Recorder::start() {
    QMetaObject::invokeMethod(qApp, [this]() {
        buffer.open(QBuffer::WriteOnly);
        recorder.start(&buffer);
    });
}

void Recorder::stop() {
    QMetaObject::invokeMethod(qApp, [this]() {
        recorder.stop();
        buffer.close();
    });
}

Player::Player(Model *model, QObject *parent)
    : BaseAudio(model, parent),
      player(setWavFormat(QAudioDeviceInfo::defaultOutputDevice())) {
    connect(&player, &QAudioOutput::notify, this, &Player::receive_audio);
    player.setNotifyInterval(50);
    connect(&player, &QAudioOutput::stateChanged, [](QAudio::State state) { qDebug() << state; });
}

void Player::start() {
    QMetaObject::invokeMethod(qApp, [this]() {
        buffer.open(QBuffer::ReadOnly);
        player.start(&buffer);
    });
}

void Player::stop() {
    QMetaObject::invokeMethod(qApp, [this]() {
        player.stop();
        buffer.close();
    });
}

void Player::receive_audio() {
//    QMetaObject::invokeMethod(qApp, [&]() { player.suspend(); });
    auto ptr = model->receive_audio_data();

    if (proceed_audio && ptr) {
        array = QByteArray(ptr->data(), static_cast<int>(ptr->size()));
        qDebug() << "player:" << array.size() << QByteArray(ptr->data(), ptr->size()).size();
    } else {
        array = QByteArray();
    }
    player.setBufferSize(array.size());
    buffer.seek(0);
//    QMetaObject::invokeMethod(qApp, [&]() { player.resume(); });
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
