#ifndef USER_H
#define USER_H

#include <QPointF>
#include <cstdint>
#include <string>
#include <utility>

class User {
    std::int64_t id_;
    std::string name_;
    QPointF pos_;

public:
    User();
    User(std::int64_t id, std::string name, const QPointF &pos);
    [[maybe_unused]] void set_pos(const QPointF &pos);
    [[maybe_unused]] [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[maybe_unused]] [[nodiscard]] QPointF pos() const;
    [[maybe_unused]] [[nodiscard]] qreal x() const;
    [[maybe_unused]] [[nodiscard]] qreal y() const;
};

#endif  // USER_H
