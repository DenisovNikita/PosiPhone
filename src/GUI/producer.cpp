#include "producer.h"
#include "model.h"

Producer::Producer(Model *model) : queue(model->getCurrentQueue()) {
}

void Producer::send_message(const Message &msg) {
    th.getEventBase()->runInEventBaseThread(
        [msg, this]() { queue->putMessage(msg); });
}
