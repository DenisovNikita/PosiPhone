#include "client.h"

namespace PosiPhone {
folly::NotificationQueue<Message> *Client::get_queue() {
    return &queue;
}

}  // namespace PosiPhone
