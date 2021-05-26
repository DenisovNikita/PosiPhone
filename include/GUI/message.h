#ifndef GUI_MESSAGE_H
#define GUI_MESSAGE_H

#include <cstdint>
#include <string>
#include <utility>

class Message {
public:
    enum MessageType {
        Empty = 0,
        Connect,
        Create,
        Move,
        AudioSource,
        AudioResult,
        Destroy,
        Check,
    };

private:
    MessageType type_;
    std::int64_t id_{};
    std::string name_;
    double x_{};
    double y_{};

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
    static Message create_by_id_name_x_y(MessageType type,
                                         std::int64_t id,
                                         std::string name,
                                         double x,
                                         double y);

public:
    Message() = default;
    [[nodiscard]] MessageType type() const;
    [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] double x() const;
    [[nodiscard]] double y() const;
    template <MessageType type, typename... Args>
    static Message create(Args &&...args) {
        if constexpr (type == MessageType::Empty) {
            return Message();
        } else if constexpr (type == MessageType::Check) {
            return create_by_id_x_y(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Connect) {
            return create_by_id_name_x_y(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Create) {
            return create_by_id_name_x_y(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Destroy ||
                             type == MessageType::AudioResult) {
            return create_by_id(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Move ||
                             type == MessageType::AudioSource) {
            return create_by_id_x_y(type, std::forward<Args>(args)...);
        }
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & type_;
        ar & id_;
        ar & name_;
        ar & x_;
        ar & y_;
    }
};

#endif  // GUI_MESSAGE_H
