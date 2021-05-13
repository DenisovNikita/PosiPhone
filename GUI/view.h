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

class View : public QWidget {
    Q_OBJECT
    Model *model = nullptr;

    QGraphicsScene scene;
    QGraphicsView view;
    std::unordered_map<std::int64_t, std::unique_ptr<MoveItem>> items;

    Player player;
    Recorder recorder;

public:
    explicit View(Model *model);

public slots:
    void add_item(const User &user, std::int64_t type);
    void remove_item(std::int64_t id);
    void set_pos(std::int64_t id, double x, double y);
};

#endif  // VIEW_H
