#include "view.h"
#include <iostream>

View::View(Model *model, QWidget *parent)
    : QMainWindow(parent), ui(), model(model), scene(this) {
    ui.setupUi(this);
    scene.setSceneRect(-300, -300, 600, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    ui.view->setScene(&scene);
    qDebug() << ui.view->mapToScene(ui.view->rect()).boundingRect();
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