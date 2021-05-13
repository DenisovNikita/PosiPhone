#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <string>
#include <utility>

struct Message {
    enum Message_type { Create, Destroy, Add, Move, Audio } type;
    const std::int64_t id;
    const std::string name;
    const double x;
    const double y;

    Message(Message_type type,
            std::int64_t id,
            std::string name,
            double x,
            double y);
};

#endif  // MESSAGE_H
