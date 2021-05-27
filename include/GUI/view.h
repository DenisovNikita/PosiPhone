#ifndef GUI_VIEW_H
#define GUI_VIEW_H

#include <glog/logging.h>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QSpacerItem>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include "audio.h"
#include "model_fwd.h"
#include "move_item.h"
#include "ui_view.h"

class View : public QMainWindow {
    Q_OBJECT
    friend Model;
    Model *model;
    bool is_shown_;
    Ui::View ui;
    Recorder *recorder;
    Player *player;
    QSpacerItem *horizontalSpacer;
    QGraphicsScene scene;
    std::unordered_map<std::int64_t, std::unique_ptr<MoveItem>> items;
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

public:
    explicit View(Model *model, QWidget *parent = nullptr);
    bool is_shown() const;

private slots:
    void add_item(const User &user, std::int64_t type);
    void remove_item(std::int64_t id);
    void set_pos(std::int64_t id, double x, double y);
};

#endif  // GUI_VIEW_H
