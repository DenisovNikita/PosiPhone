#pragma once

#include <zmq.hpp>
#include "message.h"
#include "server.h"

void send(zmq::socket_t &socket, Message &&msg);

Message receive(zmq::socket_t &socket);
