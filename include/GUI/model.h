#ifndef GUI_MODEL_H
#define GUI_MODEL_H

#include <folly/experimental/ThreadedRepeatingFunctionRunner.h>
#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <QMessageBox>
#include <QWidget>
#include <QString>
#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include "client.h"
#include "login_widget.h"
#include "message.h"
#include "model_fwd.h"
#include "move_item.h"
#include "user.h"
#include "view.h"

class Model final : public QWidget,
                    public folly::NotificationQueue<Message>::Consumer {
    Q_OBJECT
    std::int64_t ID;
    Client client;
    LoginWidget login_widget;
    View view;
    folly::ScopedEventBaseThread th;
    const uint32_t maxSize = 10'000;
    folly::NotificationQueue<Message> queue;
    std::unordered_map<std::int64_t, std::unique_ptr<User>> users;
    folly::ThreadedRepeatingFunctionRunner runner;

    void connect_to_view(View *view) const;
    void add_item(std::int64_t id,
                  const std::string &name,
                  double x,
                  double y,
                  int type);
    void remove_item(std::int64_t id);
    void set_pos(std::int64_t id, double x, double y);

signals:
    void add_item_signal(const User &user, int type);
    void remove_item_signal(std::int64_t id);
    void set_pos_signal(std::int64_t id, double x, double y);
    void disconnection_signal();
    void login_found();

private slots:
    void disconnection();

public slots:
    void check_login(const QString &login);

public:
    Model();
    void messageAvailable(Message &&msg) noexcept override;
    std::int64_t get_id() const;
    folly::NotificationQueue<Message> *getCurrentQueue();
    ~Model() override;
};

#endif  // GUI_MODEL_H
