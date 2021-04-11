#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

class MoveItem : public QGraphicsItem {
    Q_INTERFACES(QGraphicsItem)

public:
    virtual ~MoveItem() = default;

protected:
    const int RADIUS = 30, DIAMETER = 2 * RADIUS, DELTA = 3;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class MyCircle : public MoveItem {
public:
    explicit MyCircle(const QPoint &);
    ~MyCircle() override;
private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void setPos(const QPointF&);
};

class OtherCircle : public MoveItem {
public:
    explicit OtherCircle(const QPoint &);
private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setPos(const QPointF&);
};

#endif // MOVEITEM_H
