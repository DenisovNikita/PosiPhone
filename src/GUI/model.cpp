#include "model.h"
#include <glog/logging.h>
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
    connect_to_login_widget(&login_widget);
    connect(this, &Model::open_view_signal, this, &Model::open_view);
    connect(this, &Model::close_view_signal, this, &Model::close_view);
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
    runner.add("Check connection", [this]() {
        if (view.is_shown() && !client.is_ok_connection()) {
            emit close_view_signal();
        }
        return std::chrono::milliseconds(5000);
    });

    login_widget.show();
}

void Model::messageAvailable(Message &&msg) noexcept {
    LOG(INFO) << "got " << msg.type() << " message\n";
    if (msg.type() == Message::Connect) {
        if (!client.is_ok_connection()) {
            emit close_view_signal();
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

void Model::connect_to_login_widget(LoginWidget *l) const {
    connect(l, &LoginWidget::check_login_signal, this, &Model::check_login);
    connect(this, &Model::login_found_signal, l, &LoginWidget::login_found);
}

void Model::check_login(const QString &login) {
    client.get_queue()->putMessage(
        Message::create<Message::Connect>(0, login.toStdString(), 0, 0));
}

void Model::login_checked(Message &&msg) {
    if (msg.id() == -1) {
        emit login_found_signal();
    } else {
        ID = msg.id();
        queue.putMessage(
            Message::create<Message::Create>(ID, msg.name(), 0, 0));
        emit open_view_signal();
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
    queue.putMessage(Message::create<Message::Destroy>(ID));
    ID = 0;
}

Model::~Model() {
    runner.stop();
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}
