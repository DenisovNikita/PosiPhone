// Install-Package boost -Version 1.55.0.16
// Install-Package boost_system-vc110 -Version 1.55.0.16

#include <zmq.hpp>
#include <sstream>
#include <iostream>
#include <thread>
#include <map>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using std::string;
using std::stringstream;
using std::cout, std::cin;

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


void send(zmq::socket_t & socket, Message message) {
    stringstream stream;
    boost::archive::text_oarchive serializer(stream);
    serializer << message;
    auto serialized = stream.str();

    zmq::message_t request (serialized.size());
    memcpy (request.data (), serialized.data(), serialized.size());
    socket.send (request);
}

Message receive(zmq::socket_t & socket) {
    Message requestMessage;

    zmq::message_t zmqRequestMessage;
    socket.recv (&zmqRequestMessage);
    string data( static_cast<char*>(zmqRequestMessage.data()), zmqRequestMessage.size());
    stringstream inputStream(data);
    boost::archive::text_iarchive deserializer(inputStream);
    deserializer >> requestMessage;

    return requestMessage;
}

int main ()
{
    auto serverName = "tcp://localhost:5555";

    zmq::context_t context (1);

    zmq::socket_t socket (context, ZMQ_REQ);

    cout << "Connecting to hello world server…" << "\n";
    socket.connect (serverName);

    Message m;
    cout << "name: ";
    cin >> m.from;

    auto send_and_receive = [&]() {
        send(socket, m);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return receive(socket);
    };

    while(true) {
        cout << "> ";
        string command;
        cin >> command;
        if (command == "help") {
            cout << "Send message:              [send] [to] [message]\n";
            cout << "View next message in chat: [get]\n";
            cout << "View participants:         [list]\n";
            continue;
        } else if (command == "send") {
            cin >> m.to >> m.message;
            auto result = send_and_receive();
            cout << "result: from=" << result.from << ", to=" << result.to
                 << ", message=" << result.message << "\n";
        } else if (command == "get") {
            m.to = "Server";
            m.message = "get";
            auto result = send_and_receive();
            if(result.from != "Server") {
                cout << result.from << ": "
                     << result.message << "\n";
            }
        } else if (command == "list") {
            m.to = "Server";
            m.message = "list";
            auto result = send_and_receive();
            assert(result.from == "Server");
            cout << "Participants, who send something\n";
            cout << result.message;
        } else {
            assert(false);
        }
    }
}