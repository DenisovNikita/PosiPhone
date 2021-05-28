#include "server_network_module.h"

namespace PosiPhone {
folly::NotificationQueue<Message> *Server_network_module::get_queue() {
    return &queue;
}

}  // namespace PosiPhone
