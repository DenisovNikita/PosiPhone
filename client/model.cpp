#include "model.h"

namespace PosiPhone {
Model::Model()
    : this_user(),
      client(this),
      thread("model queue thread"),
      queue(maxSize),
      audio_queue(maxSize),
      login_widget(this),
      view(this),
      recorder_runner(this),
      player_runner(this) {
    qRegisterMetaType<std::int64_t>("std::int64_t");
    qRegisterMetaType<PosiPhone::User>("PosiPhone::User");

    connect_to_view(&view);
    connect_to_login_widget(&login_widget);
    connect(this, &Model::open_view_signal, this, &Model::open_view);
    connect(this, &Model::close_view_signal, this, &Model::close_view);
    auto *eventBase = thread.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
    runner.add("Check connection", [this]() {
        if (view.is_shown() && !client.is_ok_connection()) {
            emit close_view_signal();
        }
        return std::chrono::milliseconds(5000);
    });

    recorder_runner.start();
    player_runner.start();
    login_widget.show();
}

void Model::messageAvailable(Message &&msg) noexcept {
    //    LOG(INFO) << "client -> model\n" << msg;
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
        client.send_message(std::move(msg));
    } else if (msg.type() == Message::Destroy) {
        remove_item(std::move(msg));
    } else {
        LOG(WARNING) << "Unknown query\n";
    }
}

std::int64_t Model::get_id() const {
    return this_user.id();
}

void Model::send_message(Message &&msg) {
    try {
        queue.tryPutMessage(std::move(msg));
    } catch (const std::exception &e) {
        LOG(ERROR) << e.what() << '\n';
    }
}

void Model::send_audio_message(Message &&msg) {
    if (msg.data() == nullptr) {
        return;
    }
    if (!audio_queue.write(std::move(msg))) {
        LOG(WARNING) << "trying to write into full audio queue\n";
    }
}

void Model::send_audio_data(const std::shared_ptr<std::vector<char>> &ptr) {
    Message msg = Message::create<Message::AudioSource>(
        this_user.id(), this_user.x(), this_user.y(), ptr);
    ClientServerTimeStamp new_stamp = msg.stamp();
    new_stamp.client_time_sent = cur_time();
    msg.set_stamp(new_stamp);
    client.send_message(std::move(msg));
}

std::shared_ptr<std::vector<char>> Model::receive_audio_data() {
    Message msg;
    audio_queue.read(msg);
    ClientServerTimeStamp new_stamp = msg.stamp();
    new_stamp.client_time_received = cur_time();
    msg.set_stamp(new_stamp);
    return msg.data();
}

void Model::connect_to_view(View *v) const {
    connect(v->ui.recorder, &RecorderButton::clicked, recorder_runner.get(),
            &Recorder::button_clicked);
    connect(v->ui.player, &PlayerButton::clicked, player_runner.get(),
            &Player::button_clicked);
    connect(this, &Model::add_item_signal, v, &View::add_item);
    connect(this, &Model::remove_item_signal, v, &View::remove_item);
    connect(this, &Model::set_pos_signal, v, &View::set_pos);
}

void Model::connect_to_login_widget(LoginWidget *l) const {
    connect(l, &LoginWidget::check_login_signal, this, &Model::check_login);
    connect(this, &Model::login_found_signal, l, &LoginWidget::login_found);
}

void Model::check_login(const QString &login) {
    client.send_message(Message::create<Message::Connect>(
        this_user.id(), login.toStdString(), 0, 0));
}

void Model::login_checked(Message &&msg) {
    if (msg.id() == -1) {
        emit login_found_signal();
    } else {
        this_user.set_id(msg.id());
        send_message(Message::create<Message::Create>(msg.id(), msg.name(),
                                                      msg.x(), msg.y()));
        emit open_view_signal();
    }
}

void Model::add_item(Message &&msg) {
    users[msg.id()] = User(msg.id(), msg.name(), msg.x(), msg.y());
    if (msg.id() == this_user.id()) {
        emit add_item_signal(users[msg.id()], MyCircle::Type);
    } else {
        emit add_item_signal(users[msg.id()], OtherCircle::Type);
    }
}

void Model::set_pos(Message &&msg) {
    auto user = users.find(msg.id());
    if (user != users.end()) {
        user->second.set_pos(msg.x(), msg.y());
        emit set_pos_signal(msg.id(), msg.x(), msg.y());
        if (msg.id() == this_user.id()) {
            client.send_message(std::move(msg));
        }
    } else {
        LOG(WARNING) << "Trying to move unknown circle\n";
    }
}

void Model::remove_item(Message &&msg) {
    auto user = users.find(msg.id());
    if (user != users.end()) {
        users.erase(user);
        emit remove_item_signal(msg.id());
        if (msg.id() == this_user.id()) {
            client.send_message(std::move(msg));
        }
    } else {
        LOG(WARNING) << "Trying to remove unknown circle\n";
    }
}

void Model::open_view() {
    login_widget.close();
    view.show();
}

void Model::close_view() {
    view.close();
    login_widget.show();
    QMessageBox::warning(nullptr, "Warning",
                         "No internet connection\n"
                         "You have to re-login");
    send_message(Message::create<Message::Destroy>(this_user.id()));
    this_user.set_id(0);
}

Model::~Model() {
    runner.stop();
    thread.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}

}  // namespace PosiPhone
