#ifndef POSIPHONE_USER_H
#define POSIPHONE_USER_H

#include <cstdint>
#include <string>
#include <utility>

namespace PosiPhone {
class User {
    std::int64_t id_;
    std::string name_;
    double x_, y_;

public:
    User();
    User(std::int64_t id, std::string name, double x, double y);
    void set_pos(double x, double y);
    [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] double x() const;
    [[nodiscard]] double y() const;
};

}  // namespace PosiPhone

#endif  // POSIPHONE_USER_H
