#include "mock.h"
#include "model.h"

Mock::Mock(Model *model)
    : model(model), t1("first circle"), t2("second circle") {
    t1.getEventBase()->runInEventBaseThread([this]() {
        send_message(Message(Message::Message_type::Create, N1, N1, 1));
        while (true) {
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, -N1 * i / D, N1, 1));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, -N1, -N1 * i / D, 1));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, N1 * i / D, -N1, 1));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, N1, N1 * i / D, 1));
            }
        }
    });

    t2.getEventBase()->runInEventBaseThread([this]() {
        send_message(Message(Message::Message_type::Create, N2, N2, 2));
        while (true) {
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, -N2 * i / D, N2, 2));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, -N2, -N2 * i / D, 2));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, N2 * i / D, -N2, 2));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, N2, N2 * i / D, 2));
            }
        }
    });
}

void Mock::send_message(const Message &msg) {
    model->getCurrentQueue()->putMessage(msg);
}
