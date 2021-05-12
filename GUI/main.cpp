#include <QApplication>
#include "mock.h"
#include "model.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
//    Player test;
//    test.play("sample.wav");
//    test.record();
//    QTimer::singleShot(5000, &test, &Recorder::stop);
    Model model;
    Mock mock(&model);
    return QApplication::exec();
}
