#ifndef POSIPHONE_AUDIO_BUTTON_H
#define POSIPHONE_AUDIO_BUTTON_H

#include <QIcon>
#include <QPushButton>
#include <string>

namespace PosiPhone {
class AudioButton : public QPushButton {
    Q_OBJECT
    QIcon icons[2];
    void set_icon();
    Q_SLOT void switch_button();

protected:
    explicit AudioButton(const std::string &name, QWidget *parent = nullptr);
};

class RecorderButton : public AudioButton {
    Q_OBJECT

public:
    explicit RecorderButton(QWidget *parent = nullptr);
};

class PlayerButton : public AudioButton {
    Q_OBJECT

public:
    explicit PlayerButton(QWidget *parent = nullptr);
};

}  // namespace PosiPhone

#endif  // POSIPHONE_AUDIO_BUTTON_H
