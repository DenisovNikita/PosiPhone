#ifndef GUI_MESSAGE_H
#define GUI_MESSAGE_H

#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace PosiPhone {
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
        Check_connection,
        Request_new_info,
    };

private:
    MessageType type_;
    std::int64_t id_;
    std::string name_;
    double x_;
    double y_;
    std::shared_ptr<std::vector<char>> data_;
    long long time_;

    Message(MessageType type,
            std::int64_t id,
            std::string name,
            double x,
            double y,
            std::shared_ptr<std::vector<char>> data);
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
                                     std::shared_ptr<std::vector<char>> data);
    static Message create_by_id_x_y_data(
        MessageType type,
        std::int64_t id,
        double x,
        double y,
        std::shared_ptr<std::vector<char>> data);

public:
    Message();
    Message(const Message &) = default;
    Message(Message &&) noexcept = default;
    Message &operator=(const Message &) = delete;
    Message &operator=(Message &&) noexcept = default;
    ~Message() noexcept = default;
    [[nodiscard]] MessageType type() const;
    [[nodiscard]] std::int64_t id() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] double x() const;
    [[nodiscard]] double y() const;
    [[nodiscard]] std::shared_ptr<std::vector<char>> data() const;
    [[nodiscard]] long long time() const;
    friend std::ostream &operator<<(std::ostream &os, const Message &msg);
    void print(const std::string &s);
    template <MessageType type, typename... Args>
    static Message create(Args &&...args) {
        if constexpr (type == MessageType::Empty) {
            return Message();
        } else if constexpr (type == MessageType::Move) {
            return create_by_id_x_y(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Connect ||
                             type == MessageType::Create) {
            return create_by_id_name_x_y(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::AudioResult ||
                             type == MessageType::AudioSource) {
            return create_by_id_data(type, std::forward<Args>(args)...);
            // } else if constexpr (type == Message::AudioSource) {
            // return create_by_id_x_y_data(type, std::forward<Args>(args)...);
        } else if constexpr (type == MessageType::Destroy ||
                             type == MessageType::Check_connection ||
                             type == MessageType::Request_new_info) {
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
        (ar) & (time_);
    }
};

}  // namespace PosiPhone

#endif  // GUI_MESSAGE_H
