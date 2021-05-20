#ifndef GUI_MESSAGE_H
#define GUI_MESSAGE_H

#include <cstdint>
#include <string>
#include <utility>

struct Message {
    enum MessageType {
        Create,
        Destroy,
        Move,
        AudioSource,
        AudioResult,
        CheckName
    } type;
    const std::int64_t id;
    const std::string name;
    const double x;
    const double y;

    Message(MessageType type,
            std::int64_t id,
            std::string name,
            double x,
            double y);
};

#endif  // GUI_MESSAGE_H