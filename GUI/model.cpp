#include "model.h"

model::model() { // TODO: where to spawn
    setSceneRect(-300, -300, 600, 600);
    setItemIndexMethod(QGraphicsScene::NoIndex);

    // TODO: pull other circles
    // this->addItem(me);

    MyCircle *item = new MyCircle({0, 0});
    addItem(item);
}
