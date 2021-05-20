#include "message.h"

Message::Message(MessageType type,
                 std::int64_t id,
                 std::string name,
                 double x,
                 double y)
    : type_(type), id_(id), name_(std::move(name)), x_(x), y_(y) {
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
