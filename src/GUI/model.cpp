#include "model.h"
#include <iostream>

Model::Model()
    : ID(0),  // TODO: get id by network
      client(this),
      login_widget(this),
      view(this),
      th("model"),
      queue(maxSize) {
    qRegisterMetaType<std::int64_t>("std::int64_t");
    qRegisterMetaType<User>("User");

    connect_to_view(&view);
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });

    connect(this, &Model::disconnection_signal, this, &Model::disconnection);
    runner.add("Check connection", [this]() {
        if (!client.is_ok_connection()) {
            emit disconnection_signal();
        }
        return std::chrono::seconds(1);
    });

    login_widget.show();
}

void Model::messageAvailable(Message &&msg) noexcept {
    if (msg.type == Message::MessageType::Create) {
        add_item(msg.id, "abacaba", msg.x, msg.y, 1);
    } else if (msg.type == Message::MessageType::Destroy) {
        remove_item(msg.id);
    } else if (msg.type == Message::MessageType::Move) {
        set_pos(msg.id, msg.x, msg.y);
    } else if (msg.type == Message::MessageType::AudioSource) {
        // TODO send to server
    } else if (msg.type == Message::MessageType::AudioResult) {
        // TODO play recording
    } else {
        std::cerr << "Unknown query\n";
    }
}

std::int64_t Model::get_id() const {
    return ID;
}

folly::NotificationQueue<Message> *Model::getCurrentQueue() {
    return &queue;
}

void Model::connect_to_view(View *v) const {
    connect(this, &Model::add_item_signal, v, &View::add_item);
    connect(this, &Model::remove_item_signal, v, &View::remove_item);
    connect(this, &Model::set_pos_signal, v, &View::set_pos);
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

void Model::disconnection() {
//    QMessageBox::warning(this, "Warning", "No internet connection");
    // TODO show warning widget
}

void Model::check_login(const QString &login) {
    bool found = false;
    std::string name = login.toStdString();
    // TODO ask server
//    client.get_queue()->putMessage(Message(Message::MessageType::CheckName, ID, "abacaba", 0, 0));
    for (auto &[id, user] : users) {
        if (user->name() == name) {
            found = true;
            break;
        }
    }
    if (found) {
        emit login_found();
    } else {
        login_widget.close();
        add_item(ID, name, 0, 0, 0);
        // TODO send creation to everybody
        view.show();
    }
}

Model::~Model() {
    // TODO send deletion to everybody
    runner.stop();
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}
