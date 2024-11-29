#pragma once

#include <QMenu>
#include <QWidget>
#include <string>
#include "sol/sol.hpp"
#include <QHash>
#include <functional>

class ContextMenu : public QMenu {

public:
    ContextMenu(QWidget *parent = nullptr);

    enum class StandardItem {
        OPEN_WITH = 0,
        PROPERTIES,
    };

    void add_item(const std::string &item_name, const sol::function &func) noexcept;
    void add_item(const StandardItem &standard_item) noexcept;
    void add_separator() noexcept;
    void remove_item(const std::string &item_name) noexcept;
    void clear_items() noexcept;

private:
    QHash<std::string, QAction *> m_action_hash;
};