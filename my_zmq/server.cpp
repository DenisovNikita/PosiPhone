#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

int main () {
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv(request);
        std::cout << "Received " << request.to_string_view() << std::endl;

        //  Do some 'work'
        sleep(1);

        //  Send reply back to client
        zmq::message_t msg(std::move(request));
        socket.send(msg);
    }
    return 0;
}
