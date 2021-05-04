#include "audio.h"

Audio::Audio(const std::string &name)
    : QPushButton(), status(true), name(name) {
    std::string path = QDir::currentPath().toStdString() + "/../icons/";
    icons[0] = path + name + "_off.png";
    icons[1] = path + name + "_on.png";
    setFixedSize(56, 48);
    setFlat(true);
    setIcon(QIcon(icons[status].c_str()));
    setIconSize(QSize(48, 40));
    connect(this, &QPushButton::clicked, this, &Audio::switch_button);
    qDebug() << std::string("button \"" + name + "\" was created").c_str();
}

void Audio::switch_button() {
    status ^= true;
    setIcon(QIcon(icons[status].c_str()));
    setIconSize(QSize(48, 40));
    qDebug() << std::string("button \"" + name + "\" was turned " +
                            (status ? "on" : "off"))
                    .c_str();
}

Player::Player() : Audio("sound"), player(std::make_unique<QMediaPlayer>()) {
}

void Player::play(const std::string &filename) {
    qDebug() << "playing file";
    std::string file = QDir::currentPath().toStdString() + "/../" + filename;
    player->setMedia(QUrl::fromLocalFile(file.c_str()));
    player->play();
}

Recorder::Recorder()
    : Audio("micro"), recorder(std::make_unique<QAudioRecorder>()) {
}

void Recorder::record() {
    qDebug() << "recording started";
    std::string file = QDir::currentPath().toStdString() + "/sample.wav";
    recorder->setOutputLocation(QUrl::fromLocalFile(file.c_str()));
    recorder->record();
}

void Recorder::stop() {
    recorder->stop();
    qDebug() << "recording stopped";
}
