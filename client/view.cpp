#include "view.h"

namespace PosiPhone {
View::View(Model *model, QWidget *parent)
    : QMainWindow(parent),
      model(model),
      is_shown_(false),
      ui(),
      scene(this) {
    ui.setupUi(this);
    scene.setSceneRect(-300, -300, 600, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    ui.view->setScene(&scene);
    // qDebug() << ui.view->mapToScene(ui.view->rect()).boundingRect();
}

void View::closeEvent(QCloseEvent *event) {
    is_shown_ = false;
    if (ui.recorder->isChecked()) {
        ui.recorder->click();
    }
    if (ui.player->isChecked()) {
        ui.player->click();
    }
}

void View::showEvent(QShowEvent *event) {
    is_shown_ = true;
    if (!ui.recorder->isChecked()) {
        ui.recorder->click();
    }
    if (!ui.player->isChecked()) {
        ui.player->click();
    }
}

bool View::is_shown() const {
    return is_shown_;
}

void View::add_item(const User &user, std::int64_t type) {
    if (type == MyCircle::Type) {
        items[user.id()] = std::make_unique<MyCircle>(user, model);
    } else if (type == OtherCircle::Type) {
        items[user.id()] = std::make_unique<OtherCircle>(user);
    } else {
        LOG(WARNING) << "Trying to create circle of unknown type\n";
        return;
    }
    scene.addItem(items[user.id()].get());
}

void View::remove_item(std::int64_t id) {
    scene.removeItem(items[id].get());
    items.erase(items.find(id));
}

void View::set_pos(std::int64_t id, double x, double y) {
    items[id]->setPos(x, y);
}

}  // namespace PosiPhone
