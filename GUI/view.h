#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsScene>
#include <QMainWindow>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <unordered_map>
#include "audio.h"
#include "model_fwd.h"
#include "move_item.h"
#include "ui_view.h"

class View : public QMainWindow {
    Q_OBJECT
    Ui::View ui;
    Model *model;
    QGraphicsScene scene;
    std::unordered_map<std::int64_t, std::unique_ptr<MoveItem>> items;

public:
    explicit View(Model *model, QWidget *parent = nullptr);

public slots:
    void add_item(const User &user, std::int64_t type);
    void remove_item(std::int64_t id);
    void set_pos(std::int64_t id, double x, double y);
};

#endif  // VIEW_H
