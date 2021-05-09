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
    Message m;
    cout << "name: ";
    cin >> m.from;
    auto send_and_receive = [&]() {
        send(socket, m);
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_SLEEP));
        return receive(socket);
    };

    // to initialize yourself on server
    m.to = "Server";
    m.message = "init";
    send_and_receive();

    auto pooler = [&](const string &user_name){
        Message poolRequest;
        poolRequest.from = user_name;
        poolRequest.to = "Server";
        poolRequest.message = "get";

        zmq::socket_t socket(context, ZMQ_REQ);
        socket.connect(server_name);

        while (true) {
            std::this_thread::sleep_for(
                    std::chrono::milliseconds(TIME_SLEEP));
            send(socket, poolRequest);
            auto result = receive(socket);
            if (result.from != "Server") {
                cout << "\n" << result.from << ": " << result.message << "\n> ";
            }
        }
    };

    std::thread t(pooler, m.from);

    while(true) {
        cout << "> ";
        string command;
        cin >> command;
        if (command == "help") {
            cout << "Send message:              [send] [to] [message]\n";
            cout << "View participants:         [list]\n";
            cout << "Exit:                      [exit]\n";
        } else if (command == "send") {
            cin >> m.to >> m.message;
            auto result = send_and_receive();
            assert(result.message == "OK");
        } else if (command == "list") {
            m.to = "Server";
            m.message = "list";
            auto result = send_and_receive();
            assert(result.from == "Server");
            cout << "Active participants:\n";
            cout << result.message;
        } else if (command == "exit") {
            m.to = "Server";
            m.message = "kill_me";
            auto result = send_and_receive();
            assert(result.message == "OK");
            break;
        } else {
            assert(false);
        }
    }
}
