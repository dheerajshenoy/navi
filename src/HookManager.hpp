#pragma once

#include <QHash>
#include <QList>
#include <QObject>
#include "sol/sol.hpp"

class HookManager : public QObject {
    Q_OBJECT
public:
    void addHook(const std::string &hook_name, const sol::function &func) noexcept;
    void clearHookFunctions(const std::string &hook_name) noexcept;
    void triggerHook(const std::string &hook_name) noexcept;

signals:
    void triggerError(const QString &reason);

private:
    QHash<std::string, QList<sol::function>> m_hooks_map;
};