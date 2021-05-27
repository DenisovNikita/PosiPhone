#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include <folly/experimental/ThreadedRepeatingFunctionRunner.h>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QBuffer>
#include <QIcon>
#include <QPushButton>
#include <stdexcept>
#include <string>
#include "message.h"
#include "model_fwd.h"

class Audio : public QPushButton {
    Q_OBJECT
    QIcon icons[2];
    void setIcon();

private slots:
    void switch_button();

protected:
    Model *model;
    Audio(const std::string &name, Model *model, QWidget *parent = nullptr);
};

class Recorder final : public Audio {
    Q_OBJECT
    QAudioInput recorder;
    QBuffer buffer;
    QByteArray &array;
    folly::ThreadedRepeatingFunctionRunner runner;

public:
    explicit Recorder(Model *model, QWidget *parent = nullptr);
    ~Recorder() override;
};

class Player final : public Audio {
    Q_OBJECT
    QAudioOutput player;
    QBuffer buffer;
    QByteArray &array;
    folly::ThreadedRepeatingFunctionRunner runner;

public:
    explicit Player(Model *model, QWidget *parent = nullptr);
    ~Player() override;
};

#endif  // GUI_BUTTON_H
