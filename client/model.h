#ifndef POSIPHONE_MODEL_H
#define POSIPHONE_MODEL_H

#include <folly/ProducerConsumerQueue.h>
#include <folly/experimental/ThreadedRepeatingFunctionRunner.h>
#include <folly/io/async/NotificationQueue.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <glog/logging.h>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include "audio.h"
#include "client.h"
#include "login_widget.h"
#include "message.h"
#include "model_fwd.h"
#include "move_item.h"
#include "user.h"
#include "view.h"

namespace PosiPhone {
class Model final : public QObject,
                    public folly::NotificationQueue<Message>::Consumer {
    Q_OBJECT
    User this_user;
    Client client;
    folly::ScopedEventBaseThread thread;
    const uint32_t maxSize = 10'000;
    folly::NotificationQueue<Message> queue;
    folly::ProducerConsumerQueue<Message> audio_queue;
    LoginWidget login_widget;
    View view;
    RecorderRunner recorder_runner;
    PlayerRunner player_runner;
    std::unordered_map<std::int64_t, User> users;
    folly::ThreadedRepeatingFunctionRunner runner;

public:
    Model();
    void messageAvailable(Message &&msg) noexcept override;
    std::int64_t get_id() const;
    void send_message(Message &&msg);
    void send_audio_message(Message &&msg);
    void send_audio_data(const std::shared_ptr<std::vector<char>> &ptr);
    std::shared_ptr<std::vector<char>> receive_audio_data();
    ~Model() override;

signals:
    void add_item_signal(const PosiPhone::User &user, int type);
    void remove_item_signal(std::int64_t id);
    void set_pos_signal(std::int64_t id, double x, double y);
    void open_view_signal();
    void close_view_signal();
    void login_found_signal();

private slots:
    void open_view();
    void close_view();
    void check_login(const QString &login);

private:
    void connect_to_view(View *view) const;
    void connect_to_login_widget(LoginWidget *l) const;
    void login_checked(Message &&msg);
    void add_item(Message &&msg);
    void remove_item(Message &&msg);
    void set_pos(Message &&msg);
};

}  // namespace PosiPhone

#endif  // POSIPHONE_MODEL_H
