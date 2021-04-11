#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
    const int x;
    const int y;
    const int id;
    const int room_id;
    // AudioFile<float> data;

    Message(int x_, int y_, int id_, int room_id/*, AudioFile<float> data_*/)
        : x(x_), y(y_), id(id_), room_id(room_id)/*, data(std::move(data_))*/ {};
};

#endif // MESSAGE_H
