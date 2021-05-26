#pragma once

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <zmq.hpp>
#include "message.h"
#include "server.h"

static const int port = 12345;

void send(zmq::socket_t &socket, Message &&msg);

Message receive(zmq::socket_t &socket);

Message send_and_receive(zmq::socket_t &socket, Message &&msg);
