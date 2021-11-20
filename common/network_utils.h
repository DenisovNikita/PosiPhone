#pragma once

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <zmq.hpp>
#include "message.h"

namespace PosiPhone {
static const int port = 12345;
static const int IO_THREADS_ = 1;

void send(zmq::socket_t &socket, Message &&msg);

Message receive(zmq::socket_t &socket);

Message send_and_receive(zmq::socket_t &socket, Message &&msg);

}  // namespace PosiPhone