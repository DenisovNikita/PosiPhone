#include "network_utils.h"

namespace PosiPhone {
void send(zmq::socket_t &socket, Message &&msg) {
    std::stringstream stream;
    {
        boost::archive::text_oarchive serializer(stream);
        (serializer) & (msg);
    }
    auto serialized = stream.str();
    zmq::message_t request(serialized.data(), serialized.size());
    socket.send(request);
}

Message receive(zmq::socket_t &socket) {
    Message request;
    zmq::message_t response;
    socket.recv(&response);
    std::stringstream stream(response.to_string());
    {
        boost::archive::text_iarchive deserializer(stream);
        (deserializer) & (request);
    }
    return request;
}

Message send_and_receive(zmq::socket_t &socket, Message &&msg) {
    send(socket, std::move(msg));
    return receive(socket);
}

}  // namespace PosiPhone
