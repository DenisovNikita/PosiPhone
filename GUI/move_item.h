#ifndef MOVE_ITEM_H
#define MOVE_ITEM_H

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <cstdint>
#include <string>
#include "producer.h"
#include "user.h"

class MoveItem : public QGraphicsItem {
    Q_INTERFACES(QGraphicsItem)
public:
    explicit MoveItem(const User &user);
    virtual void set_pos(const QPointF &pos);

protected:
    std::int64_t ID;
    std::string name;
    const int RADIUS = 30, DIAMETER = 2 * RADIUS, DELTA = 3;
    [[nodiscard]] QRectF boundingRect() const override;
    [[nodiscard]] QPainterPath shape() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

class MyCircle : public MoveItem {
    Producer producer;

public:
    MyCircle(const User &user, Model *model);
    ~MyCircle() override;

private:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void set_pos(const QPointF &pos) override;
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
