#ifndef GUI_LOGIN_WIDGET_H
#define GUI_LOGIN_WIDGET_H

#include <QDialog>
#include <QMessageBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <string>
#include "model_fwd.h"

class LoginWidget : public QDialog {
    Q_OBJECT
    Model *model;
    QLineEdit line_edit;
    QPushButton button;

signals:
    void check_login_signal(const QString &login);

private slots:
    void button_clicked();
    void login_found();

public:
    explicit LoginWidget(Model *model);
};

#endif  // GUI_LOGIN_WIDGET_H
