#pragma once

using std::string;

struct Message {
    string from;
    string to;
    string message;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & from;
        ar & to;
        ar & message;
    }
};

void send(zmq::socket_t & socket, const Message &message);

Message receive(zmq::socket_t &socket);
