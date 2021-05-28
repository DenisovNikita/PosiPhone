#include "message.h"
#include <iostream>
#include "network_utils.h"

namespace {
long long cur_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
}  // namespace

namespace PosiPhone {
Message::Message() : type_(), id_(), name_(), x_(), y_(), data_(), time_() {
}

Message::Message(MessageType type,
                 std::int64_t id,
                 std::string name,
                 double x,
                 double y,
                 const char *data,
                 int size)
    : type_(type),
      id_(id),
      name_(std::move(name)),
      x_(x),
      y_(y),
      time_(cur_time()) {
    data_.assign(data, data + size);
}

Message::MessageType Message::type() const {
    return type_;
}

std::int64_t Message::id() const {
    return id_;
}

std::string Message::name() const {
    return name_;
}

double Message::x() const {
    return x_;
}

double Message::y() const {
    return y_;
}

const char *Message::data() const {
    return data_.data();
}

int Message::size() const {
    return static_cast<int>(data_.size());
}

long long Message::time() const {
    return time_;
}

Message Message::create_by_id(MessageType type, std::int64_t id) {
    return Message(type, id, {}, {}, {}, {}, {});
}

Message Message::create_by_id_x_y(MessageType type,
                                  std::int64_t id,
                                  double x,
                                  double y) {
    return Message(type, id, {}, x, y, {}, {});
}

Message Message::create_by_id_name_x_y(MessageType type,
                                       std::int64_t id,
                                       std::string name,
                                       double x,
                                       double y) {
    return Message(type, id, std::move(name), x, y, {}, {});
}

Message Message::create_by_id_data(MessageType type,
                                   std::int64_t id,
                                   const char *data,
                                   int size) {
    return Message(type, id, {}, {}, {}, data, size);
}

Message Message::create_by_id_x_y_data(MessageType type,
                                       std::int64_t id,
                                       double x,
                                       double y,
                                       const char *data,
                                       int size) {
    return Message(type, id, {}, x, y, data, size);
}

std::ostream &operator<<(std::ostream &os, const Message &msg) {
    os << "Got " << to_string[msg.type_] << " message\n";
    os << "id = " << msg.id_ << "\n";
    os << "x  = " << msg.x_ << "\n";
    os << "y  = " << msg.y_ << "\n";
    os << "name = " << msg.name_ << "\n";
    os << "create_time  = " << msg.time_ << "\n";
    return os;
}

}  // namespace PosiPhone
