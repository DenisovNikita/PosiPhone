#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <zmq.hpp>
#include <boost/archive/text_oarchive.hpp>

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

std::vector<Message> queue;
std::unordered_set<string> participants;

int main() {
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (true) {
        try {
            Message requestMessage;

            zmq::message_t zmqRequestMessage;
            socket.recv (&zmqRequestMessage);
            string data( static_cast<char*>(zmqRequestMessage.data()), zmqRequestMessage.size());
            stringstream inputStream(data);
            boost::archive::text_iarchive deserializer(inputStream);
            deserializer >> requestMessage;

            Message responseMessage;
            responseMessage.from = "Server";
            responseMessage.to = requestMessage.from;
            responseMessage.message = "OK";

            if (requestMessage.to == "Server") {
                if (requestMessage.message == "init") {
                    participants.insert(requestMessage.from);
                } else if (requestMessage.message == "get") {
                    auto it = find_if(queue.begin(), queue.end(),
                                      [&](Message &m) {
                                          return m.to == requestMessage.from;
                                      });

                    if (it != queue.end()) {
                        responseMessage = *it;
                        queue.erase(it);
                    } else {
                        responseMessage.message = "No new messages";
                    }
                } else if (requestMessage.message == "list") {
                    responseMessage.message.clear();
                    for (const auto &p : participants) {
                        responseMessage.message += p + "\n";
                    }
                } else if (requestMessage.message == "kill_me") {
                    participants.erase(requestMessage.from);
                } else {
                    assert(false);
                }
            } else {
                cout << "Received Hello "
                     << requestMessage.from << " -> "
                     << requestMessage.to << ": "<< requestMessage.message << "\n";


                queue.push_back(requestMessage);
            }


            stringstream outputStream;
            boost::archive::text_oarchive serializer(outputStream);
            serializer << responseMessage;
            auto serialized = outputStream.str();

            zmq::message_t response (serialized.size());
            memcpy (response.data (), serialized.data(), serialized.size());
            socket.send (response);

        } catch (const std::exception &e) {
            cout << e.what() << "\n";
        }
    }
}