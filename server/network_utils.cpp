#include "network_utils.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>
#include <zmq.hpp>
#include "server.h"

void send(zmq::socket_t &socket, Message &&msg) {
    std::stringstream stream;
    boost::archive::text_oarchive serializer(stream);
    serializer << msg;
    auto serialized = stream.str();
    zmq::message_t request(serialized.size());
    memcpy(request.data(), serialized.data(), serialized.size());
    socket.send(request);
}

Message receive(zmq::socket_t &socket) {
    Message requestMessage;
    zmq::message_t zmqResponseMessage;
    socket.recv(&zmqResponseMessage);
    std::string data(static_cast<char *>(zmqResponseMessage.data()),
                     zmqResponseMessage.size());
    std::stringstream inputStream(data);
    boost::archive::text_iarchive deserializer(inputStream);
    deserializer >> requestMessage;
    return requestMessage;
}


