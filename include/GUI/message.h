#ifndef GUI_MESSAGE_H
#define GUI_MESSAGE_H

#include <cstdint>
#include <string>
#include <utility>

class Message {
public:
    enum MessageType {
        Create,
        Destroy,
        Move,
        AudioSource,
        AudioResult,
        CheckName
    } type_;

private:
    const std::int64_t id_;
    const std::string name_;
    const double x_;
    const double y_;

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
    template <typename... Args>
    static Message create(MessageType&& type, Args&&... args) {
        if (type == MessageType::Create) {
            return create_by_id_name_x_y(type, std::forward<Args>(args)...);
        } else if (type == MessageType::Destroy ||
                   type == MessageType::AudioResult) {
            return create_by_id(type, std::forward<Args>(args)...);
        } else if (type == MessageType::Move || type == MessageType::AudioSource) {
            return create_by_id_x_y(type, std::forward<Args>(args)...);
        } else if (type == MessageType::CheckName) {
            return create_by_id_name(type, std::forward<Args>(args)...);
        } else {
        }
    }
};

#endif  // GUI_MESSAGE_H
