#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <string>
#include "model_fwd.h"

#include <QErrorMessage>

class LoginWidget : public QDialog {
    Q_OBJECT
    Model *model;
    QLineEdit line_edit;
    QPushButton button;
    QErrorMessage msg;

signals:
    void send_check_login_request(const QString &login);

private slots:
    void button_is_clicked();
    void get_check_login_result();

public:
    explicit LoginWidget(Model *model);
};

#endif  // LOGIN_WIDGET_H
