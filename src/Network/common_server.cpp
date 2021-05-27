#include "server_network_module.h"

folly::NotificationQueue<Message> *Server_network_module::get_queue() {
    return &queue;
}
