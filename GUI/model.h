#ifndef MODEL_H
#define MODEL_H

#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <cstdlib>
#include <memory>
#include "message.h"
#include "model_fwd.h"
#include "move_item.h"
#include "view_fwd.h"

class Model : public folly::NotificationQueue<Message>::Consumer {
    std::size_t ID;
    View *view;
    folly::ScopedEventBaseThread th;
    const uint32_t maxQueueSize = 10'000;
    folly::NotificationQueue<Message> queue;

public:
    explicit Model(View *view);
    void messageAvailable(Message &&msg) noexcept override;
    ~Model() override;
};

#endif  // MODEL_H
