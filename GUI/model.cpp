#include "model.h"
#include <iostream>
#include "view.h"

Model::Model(View *view)
    : ID(0),  // TODO: get id by network
      th("model"),
      queue(maxQueueSize) {
    qRegisterMetaType<std::int64_t>("std::int64_t");
    qRegisterMetaType<User>("User");
    connect(view);
    add_item(ID, QPointF(0, 0), 0);
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
}

void Model::messageAvailable(Message &&msg) noexcept {
    if (msg.type == Message::Message_type::Create) {
        add_item(msg.id, QPointF(msg.x, msg.y), 1);
        // TODO: send my coords back
    } else if (msg.type == Message::Message_type::Destroy) {
        remove_item(msg.id);
    } else if (msg.type == Message::Message_type::Add) {
        add_item(msg.id, QPointF(msg.x, msg.y), 1);
    } else if (msg.type == Message::Message_type::Move) {
        set_pos(msg.id, QPointF(msg.x, msg.y));
    } else if (msg.type == Message::Message_type::Audio) {
        // TODO
    } else {
        std::cerr << "Unknown query";
    }
}

Model::~Model() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}

void Model::connect(View *view) const {
    QObject::connect(this, &Model::add_item_signal, view, &View::add_item);
    QObject::connect(this, &Model::remove_item_signal, view, &View::remove_item);
    QObject::connect(this, &Model::set_pos_signal, view, &View::set_pos);
}

void Model::add_item(std::int64_t id, const QPointF &pos, int type) {
    users[id] = std::make_unique<User>(id, "abacaba", pos);
    emit add_item_signal(*users[id], type);
}

void Model::remove_item(std::int64_t id) {
    auto user = users.find(id);
    if (user != users.end()) {
        users.erase(user);
        emit remove_item_signal(id);
    } else {
        std::cerr << "Trying to remove unknown circle";
    }
}

void Model::set_pos(std::int64_t id, const QPointF &pos) {
    auto user = users.find(id);
    if (user != users.end()) {
        user->second->set_pos(pos);
        emit set_pos_signal(id, pos);
    } else {
        std::cerr << "Trying to move unknown circle";
    }
}
