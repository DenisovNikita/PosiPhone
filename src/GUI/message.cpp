#include "message.h"

Message::Message(Message_type type,
                 std::int64_t id,
                 std::string name,
                 double x,
                 double y)
    : type(type), id(id), name(std::move(name)), x(x), y(y) {
}