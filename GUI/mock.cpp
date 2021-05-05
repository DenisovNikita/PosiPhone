#include "mock.h"
#include "model.h"

Mock::Mock(Model *model) : model(model), th("mock") {
    th.getEventBase()->runInEventBaseThread([&]() {
        const int N1 = 150, N2 = 75, TIME = 10, D = 25;
        send_message(Message::Message_type::Create, N1, N1, 1);
        send_message(Message::Message_type::Create, N2, N2, 2);
        /*while (true) {
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message::Message_type::Move, -N1 * i / D, N1, 1);
                send_message(Message::Message_type::Move, -N2 * i / D, N2, 2);
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message::Message_type::Move, -N1, -N1 * i / D, 1);
                send_message(Message::Message_type::Move, -N2, -N2 * i / D, 2);
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message::Message_type::Move, N1 * i / D, -N1, 1);
                send_message(Message::Message_type::Move, N2 * i / D, -N2, 2);
            }
            for (int i = -D; i < D; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIME));
                send_message(Message::Message_type::Move, N1, N1 * i / D, 1);
                send_message(Message::Message_type::Move, N2, N2 * i / D, 2);
            }
        }*/
    });
}

void Mock::send_message(Message::Message_type type, int x, int y, int id) {
    model->getCurrentQueue()->putMessage(Message(type, x, y, id, -1));
}
