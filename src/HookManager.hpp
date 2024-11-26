#pragma once

#include <QHash>
#include <QList>
#include "sol/sol.hpp"

class HookManager {

public:
    void addHook(const std::string &hook_name, const sol::function &func) noexcept;
    void clearHookFunctions(const std::string &hook_name) noexcept;
    void triggerHook(const std::string &hook_name) noexcept;

private:
    QHash<std::string, QList<sol::function>> m_hooks_map;
};