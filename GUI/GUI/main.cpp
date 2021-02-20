#include "model.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    model scene;

    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.resize(600, 600);
    view.setFixedSize(600, 600);
    view.show();

    return app.exec();
}
