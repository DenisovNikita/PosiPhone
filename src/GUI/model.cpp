#include "model.h"
#include <iostream>

Model::Model()
    : ID(0),  // TODO: get get_id by network
      login_widget(this),
      view(std::make_unique<View>(this)),
      th("model"),
      queue(maxSize) {
    qRegisterMetaType<std::int64_t>("std::int64_t");
    qRegisterMetaType<User>("User");

    connect(view.get());
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });

    login_widget.show();
}

void Model::messageAvailable(Message &&msg) noexcept {
    if (msg.type == Message::Message_type::Create) {
        add_item(msg.id, "abacaba", msg.x, msg.y, 1);
    } else if (msg.type == Message::Message_type::Destroy) {
        remove_item(msg.id);
    } else if (msg.type == Message::Message_type::Move) {
        set_pos(msg.id, msg.x, msg.y);
    } else if (msg.type == Message::Message_type::Audio) {
        // TODO
    } else {
        std::cerr << "Unknown query\n";
    }
}

Model::~Model() {
    // TODO send deletion to all
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}

std::int64_t Model::get_id() const {
    return ID;
}

folly::NotificationQueue<Message> *Model::getCurrentQueue() {
    return &queue;
}

void Model::connect(View *v) const {
    QObject::connect(this, &Model::add_item_signal, v, &View::add_item);
    QObject::connect(this, &Model::remove_item_signal, v, &View::remove_item);
    QObject::connect(this, &Model::set_pos_signal, v, &View::set_pos);
}

void Model::add_item(std::int64_t id,
                     const std::string &name,
                     double x,
                     double y,
                     int type) {
    users[id] = std::make_unique<User>(id, name, x, y);
    emit add_item_signal(*users[id], type);
}

void Model::remove_item(std::int64_t id) {
    auto user = users.find(id);
    if (user != users.end()) {
        users.erase(user);
        emit remove_item_signal(id);
    } else {
        std::cerr << "Trying to remove unknown circle\n";
    }
}

void Model::set_pos(std::int64_t id, double x, double y) {
    auto user = users.find(id);
    if (user != users.end()) {
        user->second->set_pos(x, y);
        emit set_pos_signal(id, x, y);
    } else {
        std::cerr << "Trying to move unknown circle\n";
    }
}

void Model::get_check_login_request(const QString &login) {
    bool found = false;
    std::string name = login.toStdString();
    for (auto &[id, user] : users) {
        if (user->name() == name) {
            found = true;
            break;
        }
    }
    if (found) {
        emit send_check_login_result();
    } else {
        login_widget.close();
        add_item(ID, name, 0, 0, 0);
        view->show();
    }
}
