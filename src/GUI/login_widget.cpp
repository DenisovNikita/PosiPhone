#include "login_widget.h"
#include "model.h"

LoginWidget::LoginWidget(Model *model)
    : QDialog(), model(model), line_edit(), button("Log In") {
    setWindowTitle("PosiPhone");
    setModal(true);

    line_edit.setMaxLength(7);
    connect(&button, &QPushButton::clicked, this, &LoginWidget::button_clicked);
    connect(this, &LoginWidget::check_login_signal, model, &Model::check_login);
    connect(model, &Model::login_found, this, &LoginWidget::login_found);

    auto *layout = new QGridLayout(this);
    layout->addWidget(&line_edit);
    layout->addWidget(&button);
    setLayout(layout);
}

void LoginWidget::button_clicked() {
    emit check_login_signal(line_edit.text());
}

void LoginWidget::login_found() {
    QMessageBox::warning(
        this, "Warning",
        std::string("User with name \"" + line_edit.text().toStdString() +
                    "\" already exists")
            .c_str());
    line_edit.clear();
}