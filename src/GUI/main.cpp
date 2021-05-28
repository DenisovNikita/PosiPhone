#include <glog/logging.h>
#include <QApplication>
#include "model.h"

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    QApplication app(argc, argv);
    PosiPhone::Model model;
    return QApplication::exec();
}
