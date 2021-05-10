#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QToolBar>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <unordered_map>
#include "audio.h"
#include "model_fwd.h"
#include "move_item.h"
#include "view_fwd.h"

class View : public QWidget {
    Q_OBJECT
    Model *model = nullptr;

    std::unique_ptr<QGraphicsScene> scene;
    std::unique_ptr<QGraphicsView> view;
    std::unordered_map<std::int64_t, std::unique_ptr<MoveItem>> items;

    std::unique_ptr<Player> player;
    std::unique_ptr<Recorder> recorder;

public:
    explicit View(QWidget *parent = nullptr);
    void set_model(Model *model_);
public slots:
    void add_item(const User &user, int type);
    void remove_item(std::int64_t id);
    void set_pos(std::int64_t id, const QPointF &pos);
};

#endif  // VIEW_H
