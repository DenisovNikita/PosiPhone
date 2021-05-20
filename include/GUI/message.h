#ifndef GUI_MESSAGE_H
#define GUI_MESSAGE_H

#include <cstdint>
#include <string>
#include <utility>

class Message {
public:
    enum MessageType {
        Connect,
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

    Message(MessageType type,
            std::int64_t id,
            std::string name,
            double x,
            double y);
    static Message create_by_id(MessageType type, std::int64_t id);
    static Message create_by_id_x_y(MessageType type,
                                    std::int64_t id,
                                    double x,
                                    double y);
    static Message create_by_id_name(MessageType type,
                                     std::int64_t id,
                                     std::string name);
    static Message create_by_id_name_x_y(MessageType type,
                                         std::int64_t id,
                                         std::string name,
                                         double x,
                                         double y);

public:
    [[nodiscard]] MessageType type() const;
    [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] double x() const;
    [[nodiscard]] double y() const;
    template <MessageType type, typename... Args>
    static Message create(Args &&...args) {
        if constexpr (type == MessageType::Connect ||
                      type == MessageType::Create) {
            return create_by_id_name_x_y(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Destroy ||
                             type == MessageType::AudioResult) {
            return create_by_id(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Move ||
                             type == MessageType::AudioSource) {
            return create_by_id_x_y(type, std::forward<Args>(args)...);
        }
    }
};

#endif  // GUI_MESSAGE_H