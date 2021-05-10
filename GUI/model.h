#ifndef MODEL_H
#define MODEL_H

#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <QObject>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <unordered_map>
#include "message.h"
#include "model_fwd.h"
#include "move_item.h"
#include "user.h"
#include "view_fwd.h"

class Model : public QObject,
              public folly::NotificationQueue<Message>::Consumer {
    Q_OBJECT
    std::int64_t ID;
    folly::ScopedEventBaseThread th;
    const uint32_t maxQueueSize = 10'000;
    folly::NotificationQueue<Message> queue;
    std::unordered_map<std::int64_t, std::unique_ptr<User>> users;

    void connect(View *view) const;
    void add_item(std::int64_t id, const QPointF &pos, int type);
    void remove_item(std::int64_t id);
    void set_pos(std::int64_t id, const QPointF &pos);

signals:
    void add_item_signal(const User &user, int type);
    void remove_item_signal(std::int64_t id);
    void set_pos_signal(std::int64_t id, const QPointF &pos);

public:
    explicit Model(View *view);
    void messageAvailable(Message &&msg) noexcept override;
    ~Model() override;
};

#endif  // MODEL_H
