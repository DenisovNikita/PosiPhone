#ifndef QUEUECONSUMER_H
#define QUEUECONSUMER_H

#include <folly/io/async/NotificationQueue.h>
#include <QGraphicsScene>
#include <cstdlib>
#include <memory>
#include "message.h"
#include "model_fwd.h"
#include "moveitem.h"

class QueueConsumer : public folly::NotificationQueue<Message>::Consumer {
    Model *model;
    folly::EventBase eventBase;
    folly::NotificationQueue<Message> queue;

public:
    explicit QueueConsumer(Model *model);
    void messageAvailable(Message &&msg) noexcept override;
    ~QueueConsumer() override;
};

#endif  // QUEUECONSUMER_H
