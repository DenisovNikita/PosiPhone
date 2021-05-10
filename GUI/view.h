#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QToolBar>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include "audio.h"
#include "move_item.h"
#include "view_fwd.h"

class View : public QWidget {
    Q_OBJECT
    std::unique_ptr<QGraphicsScene> scene;
    std::unique_ptr<QGraphicsView> view;
    std::unordered_map<std::int64_t, std::unique_ptr<MoveItem>> items;

    std::unique_ptr<Player> player;
    std::unique_ptr<Recorder> recorder;

public:
    explicit View(QWidget *parent = nullptr);
    void addItem(std::int64_t id, std::unique_ptr<MoveItem> item);
public slots:
    void removeItem(std::int64_t id);
    void setPos(std::int64_t id, const QPointF &pos);
};

#endif  // VIEW_H
