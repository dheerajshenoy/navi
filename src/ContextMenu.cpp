#include "ContextMenu.hpp"

ContextMenu::ContextMenu(QWidget *parent) : QMenu(parent) {}

void ContextMenu::add_item(const std::string &item_name,
                           const sol::function &func) noexcept {
    QAction *action = new QAction(QString::fromStdString(item_name));
    this->addAction(action);
    connect(action, &QAction::triggered, this, [func]() {
        func();
    });
}

void ContextMenu::add_item(const StandardItem &standard_item) noexcept {
    std::string item_name;
    std::function func = []() {};
    switch (standard_item) {
    case StandardItem::OPEN_WITH:
        item_name = "Open With";
        func = []() {};
    case StandardItem::PROPERTIES:
        item_name = "Properties";
        func = []() {};
      break;
    }
    QAction *action = new QAction(QString::fromStdString(item_name));
    this->addAction(action);
    connect(action, &QAction::triggered, this, [func]() {
        func();
    });
}

void ContextMenu::add_separator() noexcept {}

void ContextMenu::remove_item(const std::string &item_name) noexcept {
    this->removeAction(m_action_hash[item_name]);
    m_action_hash.remove(item_name);
}

void ContextMenu::clear_items() noexcept {
    this->clear();
}