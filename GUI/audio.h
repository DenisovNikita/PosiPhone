#ifndef BUTTON_H
#define BUTTON_H

#include <QAudioRecorder>
#include <QDebug>
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
    std::string name;
    QIcon icons[2];
    void setIcon();

private slots:
    void switch_button();

protected:
    explicit Audio(const std::string &name, QWidget *parent = nullptr);
};

class Player : public Audio {
    Q_OBJECT
    QMediaPlayer player;
    void play(const std::string &filename);

public:
    explicit Player(QWidget *parent = nullptr);
};

class Recorder : public Audio {
    Q_OBJECT
    QAudioRecorder recorder;
    void record();
    void stop();

public:
    explicit Recorder(QWidget *parent = nullptr);
};

#endif  // BUTTON_H
