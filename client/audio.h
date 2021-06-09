#ifndef POSIPHONE_AUDIO_H
#define POSIPHONE_AUDIO_H

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QDebug>
#include <QThread>
#include <stdexcept>
#include "audio_buffer.h"
#include "message.h"
#include "model_fwd.h"

namespace PosiPhone {
class BaseAudio : public QObject {
    Q_OBJECT
protected:
    bool proceed_audio;
    Model *model;
    QBuffer buffer;
    QByteArray array;
//    AudioBuffer buffer;
    explicit BaseAudio(Model *model, QObject *parent = nullptr);

public slots:
    void button_clicked();
};

class Recorder : public BaseAudio {
    Q_OBJECT
    QAudioInput recorder;
    Q_SLOT void send_audio();

public:
    explicit Recorder(Model *model, QObject *parent = nullptr);
    void start();
    void stop();
};

class Player : public BaseAudio {
    Q_OBJECT
    QAudioOutput player;
    Q_SLOT void receive_audio();

public:
    explicit Player(Model *model, QObject *parent = nullptr);
    void start();
    void stop();
};

class RecorderRunner : public QObject {
    Q_OBJECT
    Recorder *recorder;
    QThread *thread;
    Q_SIGNAL void finished();

public:
    explicit RecorderRunner(Model *model, QObject *parent = nullptr);
    void start();
    [[nodiscard]] BaseAudio *get() const;
    ~RecorderRunner() noexcept override;
};

class PlayerRunner : public QObject {
    Q_OBJECT
    QThread *thread;
    Player *player;
    Q_SIGNAL void finished();

public:
    explicit PlayerRunner(Model *model, QObject *parent = nullptr);
    void start();
    [[nodiscard]] BaseAudio *get() const;
    ~PlayerRunner() noexcept override;
};

}  // namespace PosiPhone

#endif  // POSIPHONE_AUDIO_H
