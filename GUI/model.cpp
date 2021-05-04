#include "model.h"
#include "view.h"

Model::Model(View *view)
    : ID(0),  // TODO: get id by network
      view(view),
      th("model"),
      queue(maxQueueSize) {
    view->addItem(ID, std::make_unique<MyCircle>(QPointF(0, 0)));
    auto *eventBase = th.getEventBase();
    eventBase->runInEventBaseThread(
        [eventBase, this]() { startConsuming(eventBase, &queue); });
}

void Model::messageAvailable(Message &&msg) noexcept {
    if (msg.type == Message::Message_type::Create) {
        view->addItem(msg.id, std::make_unique<OtherCircle>(QPointF(msg.x, msg.y)));
        // TODO: send my coords back
    } else if (msg.type == Message::Message_type::Destroy) {
        view->removeItem(msg.id);
    } else if (msg.type == Message::Message_type::Add) {
        view->addItem(msg.id, std::make_unique<OtherCircle>(QPointF(msg.x, msg.y)));
    } else if (msg.type == Message::Message_type::Move) {
        view->setPos(msg.id, QPointF(msg.x, msg.y));
    } else if (msg.type == Message::Message_type::Audio) {
        // TODO
    } else {
        std::abort();
    }
}

Model::~Model() {
    th.getEventBase()->runInEventBaseThread([this]() { stopConsuming(); });
}
