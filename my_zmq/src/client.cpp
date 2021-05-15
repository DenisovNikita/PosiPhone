// Install-Package boost -Version 1.55.0.16
// Install-Package boost_system-vc110 -Version 1.55.0.16

#include <zmq.hpp>
#include <sstream>
#include <iostream>
#include <thread>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "my_utils.h"

using std::string;
using std::stringstream;
using std::cout, std::cin;

int main() {
    auto server_name = "tcp://localhost:5555";
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    cout << "Connecting to server..." << "\n";
    socket.connect(server_name);
    Message msg;
    cout << "name: ";
    cin >> msg.from;
    auto send_and_receive = [&]() {
        send(socket, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_SLEEP));
        return receive(socket);
    };

    // to initialize yourself on server
    msg.to = "Server";
    msg.command = "init";
    send_and_receive();

    auto pooler = [&](const string &user_name) {
        zmq::context_t context(1);
        Message poolRequest;
        poolRequest.from = user_name;
        poolRequest.to = "Server";
        poolRequest.command = "get";

        zmq::socket_t socket(context, ZMQ_REQ);
        socket.connect(server_name);

        while (true) {
            std::this_thread::sleep_for(
                    std::chrono::milliseconds(TIME_SLEEP));
            send(socket, poolRequest);
            auto result = receive(socket);
            if (result.from != "Server") {
                cout << "\n" << result.from << ": " << result.message << "\n> ";
                cout.flush();
            }
        }
    };

    std::thread t(pooler, msg.from);

    while (true) {
        cout << "> ";
        string command;
        cin >> command;
        if (command == "send") {
            cin >> msg.to >> msg.message;
            auto result = send_and_receive();
        } else if (command == "list") {
            msg.to = "Server";
            msg.command = "list";
            auto result = send_and_receive();
            cout << "Active participants:\n";
            cout << result.message;
        } else if (command == "exit") {
            msg.to = "Server";
            msg.command = "kill_me";
            auto result = send_and_receive();
            break;
        } else {
            cout << "Send message:              [send] [to] [message]\n";
            cout << "View participants:         [list]\n";
            cout << "Exit:                      [exit]\n";
        }
    }
    t.detach();
}
