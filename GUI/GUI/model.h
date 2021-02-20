#ifndef MODEL_H
#define MODEL_H

#include "moveitem.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

class model : public QGraphicsScene {
public:
    explicit model();
    ~model() = default;

private:
    MyCircle *me;
};
#endif // MODEL_H
