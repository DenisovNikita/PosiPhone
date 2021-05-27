#pragma once

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <zmq.hpp>
#include "message.h"

static const int port = 1234;
static const int IO_THREADS_ = 1;

static std::unordered_map<int, std::string> to_string = {
    {Message::MessageType::Empty, "Empty"},
    {Message::MessageType::Connect, "Connect"},
    {Message::MessageType::Create, "Create"},
    {Message::MessageType::Move, "Move"},
    {Message::MessageType::AudioSource, "AudioSource"},
    {Message::MessageType::AudioResult, "AudioResult"},
    {Message::MessageType::Destroy, "Destroy"},
    {Message::MessageType::Check, "Check"},
};

void send(zmq::socket_t &socket, Message &&msg);

Message receive(zmq::socket_t &socket);

Message send_and_receive(zmq::socket_t &socket, Message &&msg);
