#include "HookManager.hpp"

void HookManager::addHook(const std::string &hook_name,
                          const sol::function &func) noexcept {
    m_hooks_map[hook_name].push_back(func);
}

void HookManager::clearHookFunctions(const std::string &hook_name) noexcept {
    m_hooks_map[hook_name].clear();
}

void HookManager::triggerHook(const std::string &hook_name) noexcept {
    if (m_hooks_map.find(hook_name) != m_hooks_map.end()) {
        for (const auto &func : m_hooks_map[hook_name]) {
            try {
                func();
            } catch (const sol::error &e) {
                emit triggerError(QString(e.what()));
            }
        }
    }
}