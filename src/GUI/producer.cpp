#include "producer.h"
#include "model.h"

namespace PosiPhone {
Producer::Producer(Model *model) : queue(model->get_queue()) {
}

void Producer::send_message(Message &&msg) {
    th.getEventBase()->runInEventBaseThread(
        [msg = std::forward<Message>(msg), this]() { queue->putMessage(msg); });
}

}  // namespace PosiPhone
