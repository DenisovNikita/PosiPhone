#include "login_widget.h"
#include "model.h"

namespace PosiPhone {
LoginWidget::LoginWidget(Model *model)
    : QDialog(), model(model), line_edit(), button("Log In") {
    setWindowTitle("PosiPhone");
    setModal(true);

    line_edit.setMaxLength(7);
    connect(&button, &QPushButton::clicked, this, &LoginWidget::button_clicked);

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

}  // namespace PosiPhone
