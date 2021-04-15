#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QDebug>

class MoveItem : public QGraphicsItem {
    Q_INTERFACES(QGraphicsItem)

public:
    ~MoveItem() override = default;

protected:
    const int RADIUS = 30, DIAMETER = 2 * RADIUS, DELTA = 3;
    [[nodiscard]] QRectF boundingRect() const override;
    [[nodiscard]] QPainterPath shape() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

class MyCircle : public MoveItem {
public:
    explicit MyCircle(const QPointF &);
    ~MyCircle() override;

private:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void setPos(const QPointF &);
};

class OtherCircle : public MoveItem {
public:
    explicit OtherCircle(int x, int y);
    ~OtherCircle() override = default;

private:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

#endif  // MOVEITEM_H
