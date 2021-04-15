#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
    enum class Message_type { Create, Destroy, Add, Move, Audio } type;
    const int x;
    const int y;
    const int id;
    const int room_id;
    // AudioFile<float> data;

    Message(int x, int y, int id, int room_id /*, AudioFile<float> data_*/)
        : x(x), y(y), id(id), room_id(room_id) /*, data(std::move(data_))*/ {};
};

#endif  // MESSAGE_H
