#ifndef POSIPHONE_MOCK_H
#define POSIPHONE_MOCK_H

#include <folly/io/async/ScopedEventBaseThread.h>
#include <memory>
#include "message.h"
#include "model_fwd.h"

namespace PosiPhone {
class Mock {
    Model *model;
    folly::ScopedEventBaseThread t1, t2;
    const int N1 = 150;
    const int N2 = 75;
    const int TIME = 10;
    const int D = 25;

    void send_message(Message &&msg);

public:
    explicit Mock(Model *model);
};

}  // namespace PosiPhone

#endif  // POSIPHONE_MOCK_H
