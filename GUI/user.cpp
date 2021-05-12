#include "user.h"

User::User() : id_(), pos_() {
}

User::User(std::int64_t id, std::string name, const QPointF &pos)
    : id_(id), name_(std::move(name)), pos_(pos) {
}

User::User(std::int64_t id, std::string name, qreal x, qreal y)
    : id_(id), name_(std::move(name)), pos_(x, y) {
}

[[maybe_unused]] void User::set_pos(const QPointF &pos) {
    pos_ = pos;
}

[[maybe_unused]] void User::set_pos(qreal x, qreal y) {
    pos_ = {x, y};
}

[[maybe_unused]] std::int64_t User::id() const {
    return id_;
}

std::string User::name() const {
    return name_;
}

[[maybe_unused]] QPointF User::pos() const {
    return pos_;
}

[[maybe_unused]] qreal User::x() const {
    return pos_.x();
}

[[maybe_unused]] qreal User::y() const {
    return pos_.y();
}
