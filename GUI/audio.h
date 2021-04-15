#ifndef BUTTON_H
#define BUTTON_H

#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioRecorder>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QMediaPlayer>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>
#include <QUrl>
#include <string>

class Audio : public QPushButton {
    Q_OBJECT
    bool status;
    std::string name;
    std::string icons[2];

    void switch_button();

protected:
    explicit Audio(const std::string &name);
    ~Audio() override = default;
};

class Player : public Audio {
    QMediaPlayer *player;
    void play(const std::string &filename);

public:
    Player();
    ~Player() override;
};

class Recorder : public Audio {
    QAudioRecorder *recorder;
    void record();
    void stop();

public:
    Recorder();
    ~Recorder() override;
};

#endif  // BUTTON_H
