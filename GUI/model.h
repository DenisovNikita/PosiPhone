#ifndef MODEL_H
#define MODEL_H

#include "queueconsumer.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>

#include <QAudioInput>
#include <QAudioOutput>
#include <QFile>

class model : public QGraphicsScene {
    folly::EventBase *eventBase;
    QueueConsumer consumer;

public:
    model();
};

#endif // MODEL_H
