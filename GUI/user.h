#ifndef USER_H
#define USER_H

#include <cstdint>
#include <string>
#include <utility>

class User {
    std::int64_t id_;
    std::string name_;
    double x_, y_;

public:
    User();
    User(std::int64_t id, std::string name, double x, double y);
    [[maybe_unused]] void set_pos(double x, double y);
    [[maybe_unused]] [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[maybe_unused]] [[nodiscard]] double x() const;
    [[maybe_unused]] [[nodiscard]] double y() const;
};

#endif  // USER_H
