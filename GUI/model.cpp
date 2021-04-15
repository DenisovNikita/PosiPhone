#include "model.h"

Model::Model()
    : main(new QWidget()),
      ID(rand()),  // TODO: get id by network
      scene(new QGraphicsScene()),
      view(new QGraphicsView()),
      recorder(new Recorder()),
      player(new Player()),
      consumer(this) {  // TODO: where to spawn
    main->setWindowTitle("PosiPhone");

    // some scene settings
    scene->setSceneRect(-300, -300, 600, 600);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // some view settings
    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    addItem(ID, std::make_unique<MyCircle>(QPointF(0, 0)));

    auto *gridLayout = new QGridLayout();
    gridLayout->addWidget(view);
    auto *buttons = new QToolBar();
    buttons->addWidget(recorder);
    buttons->addWidget(player);
    gridLayout->addWidget(buttons);
    main->setLayout(gridLayout);
    main->show();
}

void Model::addItem(std::size_t id, std::unique_ptr<MoveItem> ptr) {
    items[id] = std::move(ptr);
    scene->addItem(items[id].get());
}

void Model::removeItem(std::size_t id) {
    scene->removeItem(items[id].get());
    items.erase(items.find(id));
}

void Model::setPos(std::size_t id, const QPointF &pos) {
    items[id]->setPos(pos);
}
