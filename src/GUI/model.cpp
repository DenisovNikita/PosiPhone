#include "model.h"
#include <iostream>

Model::Model()
    : ID(),
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
        //        if (!client.is_ok_connection()) {
        //            emit disconnection_signal();
        //        }
        return std::chrono::seconds(1);
    });

    login_widget.show();
//    add_item(Message::create<Message::MessageType::Create>(ID, "a)", 0, 0),
//             MyCircle::Type);
//    view.show();
}

void Model::messageAvailable(Message &&msg) noexcept {
    if (msg.type() == Message::Connect) {
        login_checked(std::move(msg));
    } else if (msg.type() == Message::Create) {
        add_item(std::move(msg), OtherCircle::Type);
    } else if (msg.type() == Message::Move) {
        set_pos(std::move(msg));
    } else if (msg.type() == Message::AudioSource) {
        client.get_queue()->putMessage(msg);
    } else if (msg.type() == Message::AudioResult) {
        // TODO play recording
    } else if (msg.type() == Message::Destroy) {
        remove_item(std::move(msg));
    }
}

folly::NotificationQueue<Message> *Model::get_queue() {
    return &queue;
}

void Model::connect_to_view(View *v) const {
    connect(this, &Model::add_item_signal, v, &View::add_item);
    connect(this, &Model::remove_item_signal, v, &View::remove_item);
    connect(this, &Model::set_pos_signal, v, &View::set_pos);
}

void Model::login_checked(Message &&msg) {
    if (msg.id() == -1) {
        emit login_found();
    } else {
        login_widget.close();
        add_item(Message::create<Message::MessageType::Create>(ID, msg.name(), 0, 0), MyCircle::Type);
        view.show();
    }
}

void Model::add_item(Message &&msg, int type) {
    if (msg.id() == ID) {
        client.get_queue()->putMessage(msg);
    }
    users[msg.id()] =
        std::make_unique<User>(msg.id(), msg.name(), msg.x(), msg.y());
    emit add_item_signal(*users[msg.id()], type);
}

void Model::remove_item(Message &&msg) {
    auto user = users.find(msg.id());
    if (user != users.end()) {
        if (msg.id() == ID) {
            client.get_queue()->putMessage(msg);
        }
        users.erase(user);
        emit remove_item_signal(msg.id());
    } else {
        std::cerr << "Trying to remove unknown circle\n";
    }
}

void Model::set_pos(Message &&msg) {
    auto user = users.find(msg.id());
    if (user != users.end()) {
        if (msg.id() == ID) {
            client.get_queue()->putMessage(msg);
        }
        user->second->set_pos(msg.x(), msg.y());
        emit set_pos_signal(msg.id(), msg.x(), msg.y());
    } else {
        std::cerr << "Trying to move unknown circle\n";
    }
}

void Model::disconnection() {
//    QMessageBox::warning(this, "Warning", "No internet connection./n You have to re-login.");
    // TODO show warning widget
}

void Model::check_login(const QString &login) {
    bool found = false;
    std::string name = login.toStdString();
    // TODO ask server
//    client.get_queue()->putMessage(
//        Message::create<Message::MessageType::Connect>(ID, name, 0, 0));
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
        add_item(Message::create<Message::MessageType::Create>(ID, name, 0, 0),
                 MyCircle::Type);
        view.show();
    }
}

Model::~Model() {
    client.get_queue()->putMessage(
        Message::create<Message::MessageType::Destroy>(ID));
    runner.stop();
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}
