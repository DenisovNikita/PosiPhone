#include <zmq.hpp>
#include <string>
#include <iostream>

int main () {
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::cout << "Connecting to char server..." << std::endl;
    socket.connect ("tcp://localhost:5555");

    while (true) {
        std::cout << "> ";
        std::cout.flush();
        std::string message;
        std::cin >> message;
        zmq::message_t msg(message);

        std::cout << "Sending message ..." << std::endl;
        socket.send (msg);
        std::cout << "Message is sent" << std::endl;

        //  Get the reply.
        zmq::message_t reply;
        socket.recv (reply);
        std::cout << "Received:  " << reply.to_string_view() << std::endl;
    }
}
