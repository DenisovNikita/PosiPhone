#include "producer.h"
#include "model.h"

Producer::Producer(Model *model) : queue(model->get_queue()) {
}

void Producer::send_message(const Message &msg) {
    th.getEventBase()->runInEventBaseThread(
        [msg, this]() { queue->putMessage(msg); });
}
