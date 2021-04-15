#include <QApplication>
#include "model.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
//    button test("sound");
//    test.play("sample.wav");
//    test.play("sample1.wav");
//    test.record();
//    QTimer::singleShot(5000, &test, &button::stop);
    Model scene;
    return app.exec();
}
