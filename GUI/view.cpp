#include "view.h"
#include <iostream>

View::View(Model *model)
    : QWidget(), model(model), scene(), view(), player(), recorder() {
    // some scene settings
    scene.setSceneRect(-300, -300, 600, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    // some view settings
    view.setScene(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setOptimizationFlags(QGraphicsView::DontSavePainterState);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *gridLayout = new QGridLayout();
    gridLayout->addWidget(&view);
    auto *buttons = new QToolBar();
    buttons->addWidget(&recorder);
    buttons->addWidget(&player);
    gridLayout->addWidget(buttons);

    setWindowTitle("PosiPhone");
    setLayout(gridLayout);
    setFixedSize(this->sizeHint());
}

void View::add_item(const User &user, std::int64_t type) {
    if (type == 0) {
        items[user.id()] = std::make_unique<MyCircle>(user, model);
    } else if (type == 1) {
        items[user.id()] = std::make_unique<OtherCircle>(user);
    } else {
        std::cerr << "Trying to create circle of unknown type\n";
        return;
    }
    scene.addItem(items[user.id()].get());
}

void View::remove_item(std::int64_t id) {
    scene.removeItem(items[id].get());
    items.erase(items.find(id));
}

void View::set_pos(std::int64_t id, double x, double y) {
    items[id]->setPos(x, y);
}
