#include <iostream>
#include <zmq.hpp>

int main()
{
    zmq::context_t ctx;
    zmq::socket_t sock_send(ctx, zmq::socket_type::push);
    zmq::socket_t sock_recv(ctx, zmq::socket_type::pull);

    const std::string address = "tcp://127.0.0.1:5000";
    sock_send.bind(address);
    sock_recv.connect(address);

    sock_send.send(zmq::str_buffer("Hello, world"), zmq::send_flags::none);

    zmq::message_t msg;
    sock_recv.recv(msg, zmq::recv_flags::none);
    std::cout << msg.to_string_view() << std::endl;
}
