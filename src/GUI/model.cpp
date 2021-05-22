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
    connect(this, &Model::close_vew_signal, this, &Model::close_view);
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
    runner.add("Check connection", [this]() {
        if (view.is_shown() /* && !client.is_ok_connection()*/) {
            emit close_vew_signal();
        }
        return std::chrono::milliseconds(5000);
    });

    login_widget.show();
//    add_item(Message::create<Message::MessageType::Create>(ID, "a)", 0, 0));
//    view.show();
}

void Model::messageAvailable(Message &&msg) noexcept {
    if (msg.type() == Message::Connect) {
        if (!client.is_ok_connection()) {
            close_view();
        } else {
            login_checked(std::move(msg));
        }
    } else if (msg.type() == Message::Create) {
        add_item(std::move(msg));
    } else if (msg.type() == Message::Move) {
        set_pos(std::move(msg));
    } else if (msg.type() == Message::AudioSource) {
        client.get_queue()->putMessage(std::move(msg));
    } else if (msg.type() == Message::AudioResult) {
        // TODO play recording
    } else if (msg.type() == Message::Destroy) {
        remove_item(std::move(msg));
    }
}

folly::NotificationQueue<Message> *Model::get_queue() {
    return &queue;
}

std::int64_t Model::get_id() const {
    return ID;
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
        ID = msg.id();
        add_item(Message::create<Message::MessageType::Create>(ID, msg.name(),
                                                               0, 0));
        view.show();
    }
}

void Model::add_item(Message &&msg) {
    users[msg.id()] =
        std::make_unique<User>(msg.id(), msg.name(), msg.x(), msg.y());
    if (msg.id() == ID) {
        emit add_item_signal(*users[msg.id()], MyCircle::Type);
    } else {
        emit add_item_signal(*users[msg.id()], OtherCircle::Type);
    }

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
        std::cerr << "Trying to remove unknown circle" << std::endl;
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
        std::cerr << "Trying to move unknown circle" << std::endl;
    }
}

void Model::open_view() {
    login_widget.close();
    view.show();
}

void Model::close_view() {
    view.close();
    login_widget.show();
    QMessageBox::warning(this, "Warning",
                         "No internet connection"
                         "\nYou have to re-login");
    remove_item(Message::create<Message::MessageType::Destroy>(ID));
    ID = 0;
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
        add_item(Message::create<Message::MessageType::Create>(ID, name, 0, 0));
        open_view();
    }
}

Model::~Model() {
    runner.stop();
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}
