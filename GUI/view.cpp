#include "view.h"
#include <iostream>

View::View(QWidget *parent)
    : QWidget(parent),
      scene(std::make_unique<QGraphicsScene>()),
      view(std::make_unique<QGraphicsView>()),
      player(std::make_unique<Player>()),
      recorder(std::make_unique<Recorder>()) {  // TODO: where to spawn
    // some scene settings
    scene->setSceneRect(-300, -300, 600, 600);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // some view settings
    view->setScene(scene.get());
    view->setRenderHint(QPainter::Antialiasing);
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *gridLayout = new QGridLayout();
    gridLayout->addWidget(view.get());
    auto *buttons = new QToolBar();
    buttons->addWidget(recorder.get());
    buttons->addWidget(player.get());
    gridLayout->addWidget(buttons);

    setWindowTitle("PosiPhone");
    setLayout(gridLayout);
    setFixedSize(this->sizeHint());
}

void View::set_model(Model *model_) {
    model = model_;
}

void View::add_item(const User &user, int type) {
    if (type == 0) {
        items[user.id()] = std::make_unique<MyCircle>(user, model);
    } else if (type == 1) {
        items[user.id()] = std::make_unique<OtherCircle>(user);
    } else {
        std::cerr << "Trying to create circle of unknown type";
        return;
    }
    scene->addItem(items[user.id()].get());
}

void View::remove_item(std::int64_t id) {
    scene->removeItem(items[id].get());
    items.erase(items.find(id));
}

void View::set_pos(std::int64_t id, const QPointF &pos) {
    items[id]->set_pos(pos);
}
