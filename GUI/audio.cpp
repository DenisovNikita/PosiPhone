#include "audio.h"

namespace {
QIcon init_icon(const std::string &name, bool b) {
    std::string path = ":/icons/" + name + (b ? "_on.png" : "_off.png");
    return QIcon(path.c_str());
}
}  // namespace

Audio::Audio(const std::string &name, QWidget *parent)
    : QPushButton(parent), name(name) {
    setCheckable(true);
    setChecked(true);
    for (int i = 0; i < 2; ++i) {
        icons[i] = init_icon(name, i);
    }
    setFixedSize(56, 48);
    setIcon();
    connect(this, &QPushButton::clicked, this, &Audio::switch_button);
    qDebug() << std::string("button \"" + name + "\" was created").c_str();
}

void Audio::setIcon() {
    QPushButton::setIcon(icons[isChecked()]);
    setIconSize(QSize(48, 40));
}

void Audio::switch_button() {
    setIcon();
    qDebug() << std::string("button \"" + name + "\" was turned " +
                            (isChecked() ? "on" : "off"))
                    .c_str();
}

Player::Player(QWidget *parent) : Audio("sound", parent), player() {
}

void Player::play(const std::string &filename) {
    qDebug() << "playing file";
    std::string file = "/../" + filename;
    player.setMedia(QUrl::fromLocalFile(file.c_str()));
    player.play();
}

Recorder::Recorder(QWidget *parent) : Audio("micro", parent), recorder() {
}

void Recorder::record() {
    qDebug() << "recording started";
    std::string file = "/sample.wav";
    recorder.setOutputLocation(QUrl::fromLocalFile(file.c_str()));
    recorder.record();
}

void Recorder::stop() {
    recorder.stop();
    qDebug() << "recording stopped";
}

//    Player test;
//    test.play("sample.wav");
//    test.record();
//    QTimer::singleShot(5000, &test, &Recorder::stop);
