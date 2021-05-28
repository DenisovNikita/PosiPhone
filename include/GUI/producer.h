#ifndef GUI_PRODUCER_H
#define GUI_PRODUCER_H

#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <utility>
#include "message.h"
#include "model_fwd.h"

namespace PosiPhone {
class Producer {
    folly::NotificationQueue<Message> *queue;
    folly::ScopedEventBaseThread th;

public:
    explicit Producer(Model *model);
    void send_message(Message &&msg);
};

}  // namespace PosiPhone

#endif  // GUI_PRODUCER_H
