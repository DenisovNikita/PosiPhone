#ifndef GUI_MOVE_ITEM_H
#define GUI_MOVE_ITEM_H

#include <QCursor>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <cstdint>
#include <string>
#include "producer.h"
#include "user.h"

class MoveItem : public QGraphicsItem {
    Q_INTERFACES(QGraphicsItem)
protected:
    explicit MoveItem(const User &user);
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
    enum { Type = UserType + 1 };

private:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

class OtherCircle : public MoveItem {
public:
    explicit OtherCircle(const User &user);
    enum { Type = UserType + 2 };

private:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

#endif  // GUI_MOVE_ITEM_H
