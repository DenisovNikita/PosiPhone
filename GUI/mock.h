#ifndef GUI_MOCK_H
#define GUI_MOCK_H

#include <folly/io/async/ScopedEventBaseThread.h>
#include <memory>
#include "message.h"
#include "model_fwd.h"

class Mock {
    Model *model;
    folly::ScopedEventBaseThread th;

    void send_message(Message::Message_type type, int x, int y, int id);

public:
    explicit Mock(Model *model);
};

#endif  // GUI_MOCK_H
