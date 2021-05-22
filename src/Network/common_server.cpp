#include "server.h"

folly::NotificationQueue<Message> *Server::get_queue() {
    return &queue;
}
