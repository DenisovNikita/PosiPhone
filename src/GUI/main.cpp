#include <QApplication>
#include "mock.h"
#include "model.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Model model;
    return QApplication::exec();
}
