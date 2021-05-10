#include "model.h"
#include "view.h"

Model::Model(View *view)
    : ID(0),  // TODO: get id by network
      view(view),
      th("model"),
      queue(maxQueueSize) {
    qRegisterMetaType<std::int64_t>("std::int64_t");
    connect(view);
    view->addItem(ID, std::make_unique<MyCircle>(User(ID, "abacaba", 0, 0)));
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
}

void Model::messageAvailable(Message &&msg) noexcept {
    if (msg.type == Message::Message_type::Create) {
        view->addItem(msg.id, std::make_unique<OtherCircle>(User(msg.id, "abacaba", msg.x, msg.y)));
        // TODO: send my coords back
    } else if (msg.type == Message::Message_type::Destroy) {
        emit remove_item_signal(msg.id);
    } else if (msg.type == Message::Message_type::Add) {
        view->addItem(msg.id, std::make_unique<OtherCircle>(User(msg.id, "abacaba", msg.x, msg.y)));
    } else if (msg.type == Message::Message_type::Move) {
        emit set_pos_signal(msg.id, QPointF(msg.x, msg.y));
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
