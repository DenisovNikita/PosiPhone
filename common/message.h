#ifndef POSIPHONE_MESSAGE_H
#define POSIPHONE_MESSAGE_H

#include <glog/logging.h>
#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace PosiPhone {

long long cur_time();

struct ClientServerTimeStamp {
    long long client_time_received;
    long long client_time_sent;
    long long server_time_received_from_client;
    long long server_time_sent_to_client;
    long long server_time_received_from_mixer;
    long long server_time_sent_to_mixer;
    long long mixer_time_received;
    long long mixer_time_sent;

    template <class Archive>
    [[maybe_unused]] void serialize(Archive &ar, const unsigned int) {
        (ar) & (client_time_received);
        (ar) & (client_time_sent);
        (ar) & (server_time_received_from_client);
        (ar) & (server_time_sent_to_client);
        (ar) & (server_time_received_from_mixer);
        (ar) & (server_time_sent_to_mixer);
        (ar) & (mixer_time_received);
        (ar) & (mixer_time_sent);
    }
};

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
    ClientServerTimeStamp stamp_;

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
    [[nodiscard]] ClientServerTimeStamp stamp() const;
    void set_stamp(ClientServerTimeStamp stamp);
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
        } else if constexpr (type == MessageType::AudioResult) {
            return create_by_id_data(type, std::forward<Args>(args)...);
        } else if constexpr (type == Message::AudioSource) {
            return create_by_id_x_y_data(type, std::forward<Args>(args)...);
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
        (ar) & (stamp_);
    }
};

}  // namespace PosiPhone

#endif  // POSIPHONE_MESSAGE_H
