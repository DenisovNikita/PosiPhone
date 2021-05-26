#include <QApplication>
#include "model.h"

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    QApplication app(argc, argv);
    Model model;
    return QApplication::exec();
}
