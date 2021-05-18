#include "login_widget.h"
#include "model.h"

LoginWidget::LoginWidget(Model *model)
    : QDialog(), model(model), line_edit(), button("Log In") {
    setWindowTitle("PosiPhone");
    setModal(true);

    line_edit.setMaxLength(7);
    connect(&button, &QPushButton::clicked, this,
            &LoginWidget::button_is_clicked);
    connect(this, &LoginWidget::send_check_login_request, model,
            &Model::get_check_login_request);
    connect(model, &Model::send_check_login_result, this,
            &LoginWidget::get_check_login_result);

    auto *layout = new QGridLayout(this);
    layout->addWidget(&line_edit);
    layout->addWidget(&button);
    setLayout(layout);
}

void LoginWidget::button_is_clicked() {
    emit send_check_login_request(line_edit.text());
}

void LoginWidget::get_check_login_result() {
    QMessageBox::warning(
        this, "Warning",
        std::string("User with name \"" + line_edit.text().toStdString() +
                    "\" already exists")
            .c_str());
    line_edit.clear();
}
