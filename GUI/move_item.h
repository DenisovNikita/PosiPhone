#ifndef MOVE_ITEM_H
#define MOVE_ITEM_H

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <string>
#include "user.h"

class MoveItem : public QGraphicsItem {
    Q_INTERFACES(QGraphicsItem)
public:
    explicit MoveItem(const User &user);

protected:
    std::string name;
    const int RADIUS = 30, DIAMETER = 2 * RADIUS, DELTA = 3;
    [[nodiscard]] QRectF boundingRect() const override;
    [[nodiscard]] QPainterPath shape() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

class MyCircle : public MoveItem {
public:
    explicit MyCircle(const User &user);
    ~MyCircle() override;

private:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void setPos(const QPointF &pos);
};

class OtherCircle : public MoveItem {
public:
    explicit OtherCircle(const User &user);

private:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

#endif  // MOVE_ITEM_H
