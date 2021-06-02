#include "user.h"

namespace PosiPhone {
User::User() : id_(), x_(), y_() {
}

User::User(std::int64_t id, std::string name, double x, double y)
    : id_(id), name_(std::move(name)), x_(x), y_(y) {
}

void User::set_pos(double x, double y) {
    x_ = x, x_ = y;
}

std::int64_t User::id() const {
    return id_;
}

std::string User::name() const {
    return name_;
}

double User::x() const {
    return x_;
}

double User::y() const {
    return y_;
}

}  // namespace PosiPhone
