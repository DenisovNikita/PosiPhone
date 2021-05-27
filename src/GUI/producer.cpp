#include "producer.h"
#include "model.h"

Producer::Producer(Model *model) : queue(model->get_queue()) {
}

void Producer::send_message(Message &&msg) {
    th.getEventBase()->runInEventBaseThread(
        [msg=std::forward<Message>(msg), this]() { queue->putMessage(msg); });
}
