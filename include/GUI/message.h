#ifndef GUI_MESSAGE_H
#define GUI_MESSAGE_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

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
    std::int64_t id_;
    std::string name_;
    double x_;
    double y_;
    std::vector<char> data_;

    Message(MessageType type,
            std::int64_t id,
            std::string name,
            double x,
            double y,
            char *data,
            int size);
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
    static Message create_by_id_data(MessageType type,
                                     std::int64_t id,
                                     char *data,
                                     int size);
    static Message create_by_id_x_y_data(MessageType type,
                                         std::int64_t id,
                                         double x,
                                         double y,
                                         char *data,
                                         int size);

public:
    Message();
    Message(const Message &) = default;
    Message(Message &&) = default;
    Message &operator=(const Message &) = default;
    Message &operator=(Message &&) = default;
    [[nodiscard]] MessageType type() const;
    [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] double x() const;
    [[nodiscard]] double y() const;
    [[nodiscard]] const char *data() const;
    [[nodiscard]] int size() const;
    template <MessageType type, typename... Args>
    static Message create(Args &&...args) {
        if constexpr (type == MessageType::Empty) {
            return Message();
        } else if constexpr (type == MessageType::Check ||
                             type == MessageType::Move) {
            return create_by_id_x_y(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Connect ||
                             type == MessageType::Create) {
            return create_by_id_name_x_y(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::AudioResult ||
                             type == MessageType::AudioSource) {
            return create_by_id_data(type, std::forward<Args>(args)...);
            // TODO pass coords to AudioSource
        } else if constexpr (type == MessageType::Destroy) {
            return create_by_id(type, std::forward<Args>(args)...);
        }
    }

    template <class Archive>
    [[maybe_unused]] void serialize(Archive &ar, const unsigned int) {
        (ar) & (type_);
        (ar) & (id_);
        (ar) & (name_);
        (ar) & (x_);
        (ar) & (y_);
        (ar) & (data_);
    }
};

#endif  // GUI_MESSAGE_H
