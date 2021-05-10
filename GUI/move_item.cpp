#include "move_item.h"

QRectF MoveItem::boundingRect() const {
    return QRectF(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
}

QPainterPath MoveItem::shape() const {
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void MoveItem::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
    painter->setPen(Qt::black);
    painter->drawEllipse(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
    painter->drawText(boundingRect(), Qt::AlignCenter, name.c_str());
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

MoveItem::MoveItem(const User &user) : QGraphicsItem(), name(user.name()) {
}

MyCircle::MyCircle(const User &user) : MoveItem(user) {
    setPos(QPointF(user.x(), user.y()));
    // TODO: push creating
}

MyCircle::~MyCircle() {
    // TODO: push deleting
}

void MyCircle::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
    painter->setBrush(Qt::green);
    MoveItem::paint(painter, option, widget);
}

void MyCircle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF pos = event->scenePos();
    pos.setX(std::max(scene()->sceneRect().topLeft().x() + RADIUS + DELTA,
             std::min(pos.x(), scene()->sceneRect().bottomRight().x() - RADIUS - DELTA)));
    pos.setY(std::max(scene()->sceneRect().topLeft().y() + RADIUS + DELTA,
             std::min(pos.y(), scene()->sceneRect().bottomRight().y() - RADIUS - DELTA)));
    bool ok = true;
    for (auto i : scene()->items()) {
        if (this == i) continue;
        qreal dist = (pos.x() - i->x()) * (pos.x() - i->x()) +
                     (pos.y() - i->y()) * (pos.y() - i->y());
        ok &= dist > DIAMETER * DIAMETER;
    }
    if (ok) setPos(pos);
}

void MyCircle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(QCursor(Qt::ClosedHandCursor));
    Q_UNUSED(event)
}

void MyCircle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event)
}

void MyCircle::setPos(const QPointF &pos) {
    QGraphicsItem::setPos(pos);
    qDebug() << "My Circle:" << pos;
    // TODO: push moving
}

OtherCircle::OtherCircle(const User &user) : MoveItem(user) {
    setPos(QPointF(user.x(), user.y()));
}

void OtherCircle::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget) {
    painter->setBrush(Qt::red);
    MoveItem::paint(painter, option, widget);
}
