#pragma once

#include "sol/sol.hpp"

void init_lua_api(sol::state &lua) noexcept;
void update_lua_package_path(sol::state &lua) noexcept;
