#ifndef POSIPHONE_AUDIO_H
#define POSIPHONE_AUDIO_H

#include <folly/experimental/ThreadedRepeatingFunctionRunner.h>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <stdexcept>
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
    explicit BaseAudio(Model *model, QObject *parent = nullptr);

public slots:
    void button_clicked();
};

class Recorder : public BaseAudio {
    Q_OBJECT
    QAudioInput recorder;
    folly::ThreadedRepeatingFunctionRunner runner;

public:
    explicit Recorder(Model *model, QObject *parent = nullptr);
    void start();
    void stop();
};

class Player : public BaseAudio {
    Q_OBJECT
    QAudioOutput player;
    folly::ThreadedRepeatingFunctionRunner runner;

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