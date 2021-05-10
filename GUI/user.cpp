#include "user.h"

User::User() : id_(), x_(), y_() {
}

User::User(std::int64_t id, std::string name, int x, int y)
    : id_(id), name_(std::move(name)), x_(x), y_(y) {
}

[[maybe_unused]] void User::set_pos(const QPointF &pos) {
    x_ = pos.x();
    y_ = pos.y();
}

[[maybe_unused]] std::int64_t User::id() const {
    return id_;
}

std::string User::name() const {
    return name_;
}

[[maybe_unused]] qreal User::x() const {
    return x_;
}

[[maybe_unused]] qreal User::y() const {
    return y_;
}
