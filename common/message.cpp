#include "message.h"
#include <iostream>

namespace {
long long cur_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
}  // namespace

namespace PosiPhone {
Message::Message()
    : type_(), id_(), name_(), x_(), y_(), data_(), time_(), stamp_() {
}

Message::Message(MessageType type,
                 std::int64_t id,
                 std::string name,
                 double x,
                 double y,
                 std::shared_ptr<std::vector<char>> data)
    : type_(type),
      id_(id),
      name_(std::move(name)),
      x_(x),
      y_(y),
      data_(std::move(data)),
      time_(cur_time()) {
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

std::shared_ptr<std::vector<char>> Message::data() const {
    return data_;
}

long long Message::time() const {
    return time_;
}

void Message::set_stamp(ClientServerTimeStamp stamp) {
    stamp_ = stamp;
}

ClientServerTimeStamp Message::stamp() const {
    return stamp_;
}

Message Message::create_by_id(MessageType type, std::int64_t id) {
    return Message(type, id, {}, {}, {}, {});
}

Message Message::create_by_id_x_y(MessageType type,
                                  std::int64_t id,
                                  double x,
                                  double y) {
    return Message(type, id, {}, x, y, {});
}

Message Message::create_by_id_name_x_y(MessageType type,
                                       std::int64_t id,
                                       std::string name,
                                       double x,
                                       double y) {
    return Message(type, id, std::move(name), x, y, {});
}

Message Message::create_by_id_data(MessageType type,
                                   std::int64_t id,
                                   std::shared_ptr<std::vector<char>> data) {
    return Message(type, id, {}, {}, {}, std::move(data));
}

Message Message::create_by_id_x_y_data(
    MessageType type,
    std::int64_t id,
    double x,
    double y,
    std::shared_ptr<std::vector<char>> data) {
    return Message(type, id, {}, x, y, std::move(data));
}

std::ostream &operator<<(std::ostream &os, const Message &msg) {
    static std::unordered_map<int, std::string> to_string;
    if (to_string.empty()) {
#define ADD_ENUM_ELEMENT(elem) (to_string[(elem)] = #elem)
        ADD_ENUM_ELEMENT(Message::Empty);
        ADD_ENUM_ELEMENT(Message::Connect);
        ADD_ENUM_ELEMENT(Message::Create);
        ADD_ENUM_ELEMENT(Message::Move);
        ADD_ENUM_ELEMENT(Message::AudioSource);
        ADD_ENUM_ELEMENT(Message::AudioResult);
        ADD_ENUM_ELEMENT(Message::Destroy);
        ADD_ENUM_ELEMENT(Message::Check_connection);
        ADD_ENUM_ELEMENT(Message::Request_new_info);
#undef ADD_ENUM_ELEMENT
    }
    os << "Got " << to_string[msg.type_] << " message\n";
    os << "id   = " << msg.id_ << "\n";
    os << "x    = " << msg.x_ << "\n";
    os << "y    = " << msg.y_ << "\n";
    os << "name = \"" << msg.name_ << "\"\n";
    os << "time = " << msg.time_ << "\n";
    return os;
}

}  // namespace PosiPhone
