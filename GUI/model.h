#ifndef MODEL_H
#define MODEL_H

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QPushButton>
#include <QToolBar>
#include <cstddef>
#include <memory>
#include <thread>
#include <unordered_map>
#include "audio.h"
#include "queueconsumer.h"

class Model {
    QWidget *main;

    std::size_t ID;
    QGraphicsScene *scene;
    QGraphicsView *view;
    std::unordered_map<std::size_t, std::unique_ptr<MoveItem>> items;

    Recorder *recorder;
    Player *player;

    folly::EventBase *eventBase;
    QueueConsumer consumer;

public:
    Model();
    void addItem(std::size_t id, std::unique_ptr<MoveItem> ptr);
    void removeItem(std::size_t id);
    void setPos(std::size_t id, const QPointF &pos);
};

#endif  // MODEL_H
