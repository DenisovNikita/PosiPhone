#include "audio_button.h"

namespace {
QIcon init_icon(const std::string &name, bool b) {
    std::string path = ":/" + name + (b ? "_on.png" : "_off.png");
    return QIcon(path.c_str());
}
}  // namespace

namespace PosiPhone {
AudioButton::AudioButton(const std::string &name, QWidget *parent)
    : QPushButton(parent) {
    setCheckable(true);
    setChecked(false);
    for (int i = 0; i < 2; ++i) {
        icons[i] = init_icon(name, i);
    }
    setFixedSize(56, 48);
    set_icon();
    connect(this, &QPushButton::clicked, this, &AudioButton::switch_button);
}

void AudioButton::set_icon() {
    setIcon(icons[isChecked()]);
    setIconSize(QSize(48, 40));
}

void AudioButton::switch_button() {
    set_icon();
}

RecorderButton::RecorderButton(QWidget *parent) : AudioButton("micro", parent) {
}

PlayerButton::PlayerButton(QWidget *parent) : AudioButton("sound", parent) {
}

}  // namespace PosiPhone
