#include "audio.h"

Audio::Audio(const std::string &name)
    : QPushButton(), status(true), name(name) {
    std::string dir = QDir::currentPath().toStdString();
    icons[0] = dir + "/../icons/" + name + "_off.png";
    icons[1] = dir + "/../icons/" + name + "_on.png";
    setFixedSize(56, 48);
    setFlat(true);
    setIcon(QIcon(icons[status].c_str()));
    setIconSize(QSize(48, 40));
    connect(this, &QPushButton::clicked, this, &Audio::switch_button);
    qDebug() << "button \"" << name.c_str() << "\" was created";
}

void Audio::switch_button() {
    status ^= true;
    setIcon(QIcon(icons[status].c_str()));
    setIconSize(QSize(48, 40));
    qDebug() << "button \"" << name.c_str() << "\" was turned "
             << (status ? "on" : "off");
}

Player::Player() : Audio("sound"), player(new QMediaPlayer()) {
}

Player::~Player() {
    delete player;
}

void Player::play(const std::string &filename) {
    qDebug() << "playing file";
    std::string file = QDir::currentPath().toStdString() + "/../" + filename;
    player->setMedia(QUrl::fromLocalFile(file.c_str()));
    player->play();
}

Recorder::Recorder() : Audio("micro"), recorder(new QAudioRecorder()) {
}

Recorder::~Recorder() {
    delete recorder;
}

void Recorder::record() {
    qDebug() << "recording started";
    recorder->setOutputLocation(QUrl::fromLocalFile(
        "/home/alexey/Desktop/projects/PosiPhone/GUI/sample1.wav"));
    recorder->record();
}

void Recorder::stop() {
    recorder->stop();
    qDebug() << "recording stopped";
}
