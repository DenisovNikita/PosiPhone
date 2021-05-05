#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <zmq.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "my_utils.h"

using std::string;
using std::stringstream;
using std::cout, std::cin;

std::vector<Message> queue;
std::unordered_set<string> participants;

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind ("tcp://*:5555");
    while (true) {
        try {
            Message requestMessage = receive(socket);
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
            send(socket, responseMessage);
        } catch (const std::exception &e) {
            cout << e.what() << "\n";
        }
    }
}
