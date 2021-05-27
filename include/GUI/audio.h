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

class Audio : public QPushButton {
    Q_OBJECT
    QIcon icons[2];
    void setIcon();

private slots:
    void switch_button();

protected:
    explicit Audio(const std::string &name, QWidget *parent = nullptr);
};

class Recorder final : public Audio {
    Q_OBJECT
    QAudioInput recorder;
    QBuffer buffer;
    folly::ThreadedRepeatingFunctionRunner runner;

public:
    explicit Recorder(QWidget *parent = nullptr);
    ~Recorder() override;
    QByteArray record();
};

class Player final : public Audio {
    Q_OBJECT
    QAudioOutput player;
    QBuffer buffer;
    folly::ThreadedRepeatingFunctionRunner runner;

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player() override;
    void play(const QByteArray &arr);
};

#endif  // GUI_BUTTON_H
