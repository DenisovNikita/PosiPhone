#ifndef MOCK_H
#define MOCK_H

#include <folly/io/async/ScopedEventBaseThread.h>
#include <memory>
#include "message.h"
#include "model_fwd.h"

class Mock {
    Model *model;
    folly::ScopedEventBaseThread t1, t2;
    const int N1 = 150, N2 = 75, TIME = 10, D = 25;

    void send_message(const Message &msg);

public:
    explicit Mock(Model *model);
};

#endif  // MOCK_H
