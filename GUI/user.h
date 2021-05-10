#ifndef USER_H
#define USER_H

#include <QPointF>
#include <cstdint>
#include <string>
#include <utility>

class User {
    std::int64_t id_;
    std::string name_;
    qreal x_, y_;

public:
    User();
    User(std::int64_t id, std::string name, int x, int y);
    [[maybe_unused]] void set_pos(const QPointF &pos);
    [[maybe_unused]] [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[maybe_unused]] [[nodiscard]] qreal x() const;
    [[maybe_unused]] [[nodiscard]] qreal y() const;
};

#endif  // USER_H
