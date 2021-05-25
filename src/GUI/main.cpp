#include <QApplication>
#include "model.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Model model;
    return QApplication::exec();
}
