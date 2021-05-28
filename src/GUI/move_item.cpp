#include "move_item.h"
#include "model.h"

namespace PosiPhone {
MoveItem::MoveItem(const User &user) : QGraphicsItem(), name(user.name()) {
}

QRectF MoveItem::boundingRect() const {
    return QRectF(-RADIUS, -RADIUS, 2 * RADIUS, 2 * RADIUS);
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
    painter->drawEllipse(boundingRect());
    painter->drawText(boundingRect(), Qt::AlignCenter, name.c_str());
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

MyCircle::MyCircle(const User &user, Model *model)
    : MoveItem(user), model(model) {
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
    model->get_queue()->putMessage(Message::create<Message::MessageType::Move>(
        model->get_id(), pos.x(), pos.y()));
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

}  // namespace PosiPhone
