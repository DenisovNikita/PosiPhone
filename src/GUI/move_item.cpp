#include "move_item.h"

MoveItem::MoveItem(const User &user)
    : QGraphicsItem(), ID(user.id()), name(user.name()) {
}

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

MyCircle::MyCircle(const User &user, Model *model)
    : MoveItem(user), producer(model) {
    setPos(QPointF(user.x(), user.y()));
}

void MyCircle::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
    painter->setBrush(Qt::green);
    MoveItem::paint(painter, option, widget);
}

void MyCircle::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF pos = event->scenePos();
    producer.send_message(
        Message(Message::MessageType::Move, ID, name, pos.x(), pos.y()));
}

void MyCircle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(QCursor(Qt::ClosedHandCursor));
    Q_UNUSED(event)
}

void MyCircle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event)
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
