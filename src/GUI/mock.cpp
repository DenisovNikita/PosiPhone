#include "mock.h"
#include "model.h"

Mock::Mock(Model *model)
    : model(model), t1("first circle"), t2("second circle") {
    t1.getEventBase()->runInEventBaseThread([this]() {
        const std::int64_t ID = INT64_MAX - 1;
        send_message(Message(Message::Message_type::Create, ID, "abacaba", N1, N1));
        while (true) {
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, ID, "abacaba", -N1 * i / D, N1));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, ID, "abacaba", -N1, -N1 * i / D));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, ID, "abacaba", N1 * i / D, -N1));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, ID, "abacaba", N1, N1 * i / D));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        send_message(Message(Message::Message_type::Destroy, ID, "abacaba", N1, N1));
    });

    t2.getEventBase()->runInEventBaseThread([this]() {
        const std::int64_t ID = INT_MAX;
        send_message(Message(Message::Message_type::Create, ID, "abacaba", N2, N2));
        while (true) {
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, ID, "abacaba", -N2 * i / D, N2));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, ID, "abacaba", -N2, -N2 * i / D));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, ID, "abacaba", N2 * i / D, -N2));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message(Message::Message_type::Move, ID, "abacaba", N2, N2 * i / D));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        send_message(Message(Message::Message_type::Destroy, ID, "abacaba", N2, N2));
    });
}

void Mock::send_message(const Message &msg) {
    model->getCurrentQueue()->putMessage(msg);
}
