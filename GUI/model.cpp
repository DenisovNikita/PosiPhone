#include "model.h"
#include "view.h"

Model::Model(View *view)
    : ID(0),  // TODO: get id by network
      view(view),
      th("model"),
      queue(maxQueueSize) {
    qRegisterMetaType<std::int64_t>("std::int64_t");
    connect(view);
    add_my_circle(ID, QPointF(0, 0));
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
}

void Model::messageAvailable(Message &&msg) noexcept {
    if (msg.type == Message::Message_type::Create) {
        add_other_circle(msg.id, QPointF(msg.x, msg.y));
        // TODO: send my coords back
    } else if (msg.type == Message::Message_type::Destroy) {
        remove_item(msg.id);
    } else if (msg.type == Message::Message_type::Add) {
        add_other_circle(msg.id, QPointF(msg.x, msg.y));
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
//    QObject::connect(this, &Model::add_item, view, &View::addItem);
    QObject::connect(this, &Model::remove_item_signal, view, &View::removeItem);
    QObject::connect(this, &Model::set_pos_signal, view, &View::setPos);
}

void Model::add_my_circle(std::int64_t id, const QPointF &pos) {
    users[id] = std::make_unique<User>(id, "abacaba", pos);
    view->addItem(id, std::make_unique<MyCircle>(*(users[id].get())));
}

void Model::add_other_circle(std::int64_t id, const QPointF &pos) {
    users[id] = std::make_unique<User>(id, "abacaba", pos);
    view->addItem(id, std::make_unique<OtherCircle>(*users[id].get()));
}

void Model::remove_item(std::int64_t id) {
    users.erase(users.find(id));
    emit remove_item_signal(id);
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
