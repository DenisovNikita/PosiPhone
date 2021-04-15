#include "moveitem.h"

QRectF MoveItem::boundingRect() const {
    return QRectF(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
}

QPainterPath MoveItem::shape() const {
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void MoveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(Qt::black);
    painter->drawEllipse(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

MyCircle::MyCircle(const QPointF &pos) : MoveItem() {
    setPos(pos);
    // TODO: push creating of my circle
}

MyCircle::~MyCircle() {
    // TODO: push deleting of my circle
}

void MyCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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
    for(auto& i : scene()->items()) {
        if (this == i) continue;
        qreal dist = (pos.x() - i->x()) * (pos.x() - i->x())
                   + (pos.y() - i->y()) * (pos.y() - i->y());
        if (dist <= DIAMETER * DIAMETER) ok = false;
    }
    if (ok) this->setPos(pos);
}

void MyCircle::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    this->setCursor(QCursor(Qt::ClosedHandCursor));
    Q_UNUSED(event)
}

void MyCircle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    this->setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event)
}

void MyCircle::setPos(const QPointF& pos) {
    this->QGraphicsItem::setPos(pos);
    qDebug() << "cur pos: " << pos;
    // TODO: push to move
}

OtherCircle::OtherCircle(int x, int y) : MoveItem() {
    setPos(QPoint(x, y));
}

void OtherCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setBrush(Qt::red);
    MoveItem::paint(painter, option, widget);
}
