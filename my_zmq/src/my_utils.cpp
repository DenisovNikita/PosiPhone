#include <zmq.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "my_utils.h"

using std::stringstream;

void send(zmq::socket_t & socket, const Message &message) {
    stringstream stream;
    boost::archive::text_oarchive serializer(stream);
    serializer << message;
    auto serialized = stream.str();
    zmq::message_t request(serialized.size());
    memcpy(request.data(), serialized.data(), serialized.size());
    socket.send(request);
}

Message receive(zmq::socket_t &socket) {
    Message requestMessage;
    zmq::message_t zmqRequestMessage;
    socket.recv(&zmqRequestMessage);
    string data(static_cast<char*>(zmqRequestMessage.data()), zmqRequestMessage.size());
    stringstream inputStream(data);
    boost::archive::text_iarchive deserializer(inputStream);
    deserializer >> requestMessage;
    return requestMessage;
}
