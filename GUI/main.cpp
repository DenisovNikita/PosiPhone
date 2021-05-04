#include <QApplication>
#include "mock.h"
#include "model.h"
#include "view.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
//    Player test;
//    test.play("sample.wav");
//    test.record();
//    QTimer::singleShot(5000, &test, &Recorder::stop);
    View view;
    Model model(&view);
    Mock mock(&model);

    view.show();
    return app.exec();
}
