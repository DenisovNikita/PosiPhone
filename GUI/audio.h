#ifndef BUTTON_H
#define BUTTON_H

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
#include <memory>
#include <string>

class Audio : public QPushButton {
    Q_OBJECT
    bool status;
    std::string name;
    std::string icons[2];

    void switch_button();

protected:
    explicit Audio(const std::string &name);
};

class Player : public Audio {
    std::unique_ptr<QMediaPlayer> player;
    void play(const std::string &filename);

public:
    Player();
};

class Recorder : public Audio {
    std::unique_ptr<QAudioRecorder> recorder;
    void record();
    void stop();

public:
    Recorder();
};

#endif  // BUTTON_H
