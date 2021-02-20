#include "model.h"

model::model() : me(MyCircle::create(QPointF(100, 100))) { // TODO: where to spawn
    this->setSceneRect(-300, -300, 600, 600);
    this->setItemIndexMethod(QGraphicsScene::NoIndex);

    // TODO: pull other circles
    this->addItem(me);
}
