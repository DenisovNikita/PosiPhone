#include "mock.h"
#include "model.h"

namespace PosiPhone {
Mock::Mock(Model *model)
    : model(model), t1("first circle"), t2("second circle") {
    t1.getEventBase()->runInEventBaseThread([this]() {
        const std::int64_t ID = INT64_MAX - 1;
        send_message(Message::create<Message::Create>(ID, "abacaba", N1, N1));
        while (true) {
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(
                    Message::create<Message::Move>(ID, -N1 * i / D, N1));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(
                    Message::create<Message::Move>(ID, -N1, -N1 * i / D));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(
                    Message::create<Message::Move>(ID, N1 * i / D, -N1));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(
                    Message::create<Message::Move>(ID, N1, N1 * i / D));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        send_message(Message::create<Message::Destroy>(ID));
    });

    t2.getEventBase()->runInEventBaseThread([this]() {
        const std::int64_t ID = INT_MAX;
        send_message(Message::create<Message::Create>(ID, "abacaba", N2, N2));
        while (true) {
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(
                    Message::create<Message::Move>(ID, -N2 * i / D, N2));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(
                    Message::create<Message::Move>(ID, -N2, -N2 * i / D));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(
                    Message::create<Message::Move>(ID, N2 * i / D, -N2));
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(
                    Message::create<Message::Move>(ID, N2, N2 * i / D));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
        send_message(Message::create<Message::Destroy>(ID));
    });
}

void Mock::send_message(Message &&msg) {
    model->send_message(std::move(msg));
}

}  // namespace PosiPhone
