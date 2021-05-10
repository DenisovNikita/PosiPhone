#include "view.h"

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

void View::addItem(int64_t id, std::unique_ptr<MoveItem> item) {
    qDebug() << "create circle at " << item->pos();
    items[id] = std::move(item);
    scene->addItem(items[id].get());
}

void View::removeItem(std::int64_t id) {
    scene->removeItem(items[id].get());
    items.erase(items.find(id));
    qDebug() << "destroy circle with number" << id;
}

void View::setPos(std::int64_t id, const QPointF &pos) {
    items[id]->setPos(pos);
    qDebug() << "Other Circle:" << pos;
}
