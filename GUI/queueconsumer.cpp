#include "queueconsumer.h"
#include "model.h"

QueueConsumer::QueueConsumer(Model *model) : model(model) {
    startConsuming(&eventBase, &queue);
    //        eventBase.loopForever();
}

void QueueConsumer::messageAvailable(Message &&msg) noexcept {
    if (msg.type == Message::Message_type::Create) {
        model->addItem(msg.id, std::make_unique<OtherCircle>(msg.x, msg.y));
        // TODO: send my coords back
    } else if (msg.type == Message::Message_type::Destroy) {
        model->removeItem(msg.id);
    } else if (msg.type == Message::Message_type::Add) {
        model->addItem(msg.id, std::make_unique<OtherCircle>(msg.x, msg.y));
    } else if (msg.type == Message::Message_type::Move) {
        model->setPos(msg.id, QPointF(msg.x, msg.y));
    } else if (msg.type == Message::Message_type::Audio) {
        // TODO
    } else {
        std::abort();
    }
}

QueueConsumer::~QueueConsumer() {
    stopConsuming();
    eventBase.terminateLoopSoon();
}
