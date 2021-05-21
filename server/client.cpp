#include "client.h"

folly::NotificationQueue<Message> *Client::get_queue() {
    return &queue;
}
