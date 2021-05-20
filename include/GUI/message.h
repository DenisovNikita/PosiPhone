#ifndef GUI_MESSAGE_H
#define GUI_MESSAGE_H

#include <cstdint>
#include <string>
#include <utility>

class Message {
public:
    enum MessageType {
        Connect, // TODO send pointer on client
        Create,
        Move,
        AudioSource,
        AudioResult,
        Destroy
    };
private:
    MessageType type_;
    std::int64_t id_;
    std::string name_;
    double x_, y_;

public:
    Message(MessageType type,
            std::int64_t id,
            std::string name,
            double x,
            double y);
    [[nodiscard]] MessageType type() const;
    [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] double x() const;
    [[nodiscard]] double y() const;
};

#endif  // GUI_MESSAGE_H