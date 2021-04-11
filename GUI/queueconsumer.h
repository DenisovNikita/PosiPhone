#ifndef QUEUECONSUMER_H
#define QUEUECONSUMER_H

#include "message.h"
#include "moveitem.h"
#include <folly/io/async/NotificationQueue.h>

class QueueConsumer : public folly::NotificationQueue<Message>::Consumer {
    folly::EventBase eventBase;
    folly::NotificationQueue<Message> queue;
    std::vector<MoveItem> items;
public:
    QueueConsumer() {
        startConsuming(&eventBase, &queue);
//        eventBase.loopForever();
    }
    void messageAvailable(Message &&value) noexcept override {
        items[value.id].setPos(value.x, value.y);
    }
    ~QueueConsumer() {
        stopConsuming();
        eventBase.terminateLoopSoon();
    }
};

#endif // QUEUECONSUMER_H
