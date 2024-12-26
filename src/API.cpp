#include "Navi.hpp"

void Navi::initNaviLuaAPI() noexcept {

    // ItemProperty type
    lua.new_usertype<FilePanel::ItemProperty>(
        "ItemProperty", "name", &FilePanel::ItemProperty::name, "size",
        &FilePanel::ItemProperty::size, "mimeName",
        &FilePanel::ItemProperty::mimeName);

    lua.new_usertype<MenuItem>("MenuItem",
                               "label", &MenuItem::label, "action",
                               sol::property([](MenuItem &m, sol::function func) {
                               m.action = [func]() { func(); };
                               }),
                               "submenu", &MenuItem::submenu);

    lua.new_usertype<Statusbar::Module>("Module",
                                        "name", &Statusbar::Module::name,
                                        "options", &Statusbar::Module::options);

    lua.new_usertype<Navi::ToolbarItem>("ToolbarItem",
                                        "label", &Navi::ToolbarItem::label,
                                        "icon", &Navi::ToolbarItem::icon,
                                        "position", &Navi::ToolbarItem::position,
                                        "action", &Navi::ToolbarItem::action);

    lua["navi"] = lua.create_table();

    // HOOK API

    lua["navi"]["hook"] = lua.create_table();

    lua["navi"]["hook"]["add"] = [this](const std::string &hook_name,
                                        const sol::function &func) noexcept {
            m_hook_manager->addHook(hook_name, func);
        };

    lua["navi"]["hook"]["trigger"] = [this](const std::string &hook_name) noexcept {
        m_hook_manager->triggerHook(hook_name);
    };

    lua["navi"]["hook"]["clear_functions"] = [this](const std::string &hook_name,
                                                    const sol::function &func) noexcept {
            m_hook_manager->clearHookFunctions(hook_name);
        };

    // BOOKMARK API
    lua["navi"]["bookmark"] = lua.create_table();

    lua["navi"]["bookmark"]["add"] = [this](const std::string &bookmark_name,
                                            const std::string &file_path,
                                            const bool &highlight) noexcept {
            m_bookmark_manager->addBookmark(QString::fromStdString(bookmark_name),
                                            QString::fromStdString(file_path),
                                            highlight);
        };

    lua["navi"]["bookmark"]["remove"] = [this](const std::string &bookmark_name) noexcept {
        m_bookmark_manager->removeBookmark(QString::fromStdString(bookmark_name));
    };

    lua["navi"]["bookmark"]["edit_name"] =
        [this](const std::string &bookmark_name,
               const std::string &new_bookmark_name) noexcept {
            m_bookmark_manager->setBookmarkName(
                QString::fromStdString(bookmark_name),
                QString::fromStdString(new_bookmark_name));
        };

    // API API
    lua["navi"]["api"] = lua.create_table();

    lua["navi"]["api"]["cd"] = [this](const std::string &dir) noexcept {
        m_file_panel->setCurrentDir(QString::fromStdString(dir), true);
    };

    lua["navi"]["api"]["count"] = [this](const std::string &path) noexcept {
        return m_file_panel->ItemCount();
    };

    lua["navi"]["api"]["create_dir"] =
        [this](const std::vector<std::string> &paths) noexcept {
            this->Lua__CreateFolders(paths);
        };

    lua["navi"]["api"]["is_file"] = [this](const std::string &path) noexcept {
        return QFileInfo(QString::fromStdString(path)).isFile();
    };

    lua["navi"]["api"]["is_dir"] = [this](const std::string &path) noexcept {
        return QFileInfo(QString::fromStdString(path)).isDir();
    };

    lua["navi"]["api"]["next_item"] = [this]() noexcept { m_file_panel->NextItem(); };

    lua["navi"]["api"]["prev_item"] = [this]() noexcept { m_file_panel->PrevItem(); };

    lua["navi"]["api"]["first_item"] = [this]() noexcept {
        m_file_panel->GotoFirstItem();
    };

    lua["navi"]["api"]["last_item"] = [this]() noexcept { m_file_panel->GotoLastItem(); };

    lua["navi"]["api"]["middle_item"] = [this]() noexcept {
        m_file_panel->GotoMiddleItem();
    };

    lua["navi"]["api"]["parent_directory"] = [this]() noexcept {
        m_file_panel->UpDirectory();
    };

    lua["navi"]["api"]["previous_directory"] = [this]() noexcept {
        m_file_panel->PreviousDirectory();
    };

    lua["navi"]["api"]["select_item"] = [this]() noexcept { m_file_panel->SelectItem(); };

    lua["navi"]["api"]["item_name"] = [this]() noexcept {
        return m_file_panel->getCurrentItem().toStdString();
    };

    lua["navi"]["api"]["item_property"] = [this]() noexcept {
        return m_file_panel->getItemProperty();
    };

    lua["navi"]["api"]["pwd"] = [this]() noexcept {
        return m_file_panel->getCurrentDir().toStdString();
    };
    lua["navi"]["api"]["filter"] = [this](const QString &filter) noexcept {
        m_file_panel->Filters(filter);
    };
    lua["navi"]["api"]["reset_filter"] = [this]() noexcept {
        m_file_panel->ResetFilter();
    };
    lua["navi"]["api"]["mark"] = [this]() noexcept { return m_file_panel->MarkItem(); };
    lua["navi"]["api"]["mark_inverse"] = [this]() noexcept {
        m_file_panel->MarkInverse();
    };
    lua["navi"]["api"]["mark_all"] = [this]() noexcept {
        return m_file_panel->MarkAllItems();
    };
    lua["navi"]["api"]["mark_dwim"] = [this]() noexcept {
        return m_file_panel->MarkDWIM();
    };
    lua["navi"]["api"]["unmark"] = [this]() noexcept { m_file_panel->UnmarkItem(); };

    lua["navi"]["api"]["unmark_dwim"] = [this]() noexcept { m_file_panel->UnmarkDWIM(); };

    lua["navi"]["api"]["toggle_mark"] = [this]() noexcept {
        m_file_panel->ToggleMarkItem();
    };

    lua["navi"]["api"]["toggle_mark_dwim"] = [this]() noexcept {
        m_file_panel->ToggleMarkDWIM();
    };
    lua["navi"]["api"]["rename"] = [this]() noexcept { m_file_panel->RenameItem(); };

    lua["navi"]["api"]["rename_dwim"] = [this]() noexcept { m_file_panel->RenameDWIM(); };

    lua["navi"]["api"]["delete"] = [this]() noexcept { m_file_panel->DeleteItem(); };

    lua["navi"]["api"]["delete_dwim"] = [this]() noexcept { m_file_panel->DeleteDWIM(); };

    lua["navi"]["api"]["chmod"] = [this]() noexcept { m_file_panel->ChmodItem(); };

    lua["navi"]["api"]["spawn"] = [this](const std::string &command,
                                         const std::vector<std::string> args) noexcept {
            QStringList arglist;

            for (const auto &str : args)
            arglist.push_back(QString::fromStdString(str));

            SpawnProcess(QString::fromStdString(command), arglist);
        };

    lua["navi"]["api"]["has_selection"] = [this]() noexcept {
        return m_file_panel->has_selection();
    };

    lua["navi"]["api"]["local_marks"] = [this]() noexcept {
        return utils::convertToStdVector(
            m_file_panel->model()->getMarkedFilesLocal());
    };

    lua["navi"]["api"]["local_marks_count"] = [this]() noexcept {
        return m_file_panel->model()->getMarkedFilesCountLocal();
    };

    lua["navi"]["api"]["global_marks"] = [this]() noexcept {
        return utils::convertToStdVector(m_file_panel->model()->getMarkedFiles());
    };

    lua["navi"]["api"]["global_marks_count"] = [this]() noexcept {
        return m_file_panel->model()->getMarkedFilesCount();
    };

    lua["navi"]["api"]["has_marks_global"] = [this]() noexcept {
        return m_file_panel->model()->hasMarks();
    };

    lua["navi"]["api"]["has_marks_local"] = [this]() noexcept {
        return m_file_panel->model()->hasMarksLocal();
    };
    lua["navi"]["api"]["cut"] = [this]() noexcept { m_file_panel->CutItem(); };

    lua["navi"]["api"]["cut_dwim"] = [this]() noexcept { m_file_panel->CutDWIM(); };

    lua["navi"]["api"]["copy_dwim"] = [this]() noexcept { m_file_panel->CopyDWIM(); };

    lua["navi"]["api"]["copy"] = [this]() noexcept { return m_file_panel->CopyItem(); };

    lua["navi"]["api"]["trash"] = [this]() noexcept { m_file_panel->TrashItem(); };

    lua["navi"]["api"]["trash_dwim"] = [this]() noexcept{ m_file_panel->TrashDWIM(); };

    lua["navi"]["api"]["new_folders"] =
        [this](const std::vector<std::string> &folderNames) noexcept {
            QStringList folders = utils::stringListFromVector(folderNames);
            m_file_panel->NewFolder(folders);
        };

    lua["navi"]["api"]["new_files"] =
        [this](const std::vector<std::string> &fileNames) noexcept {
            QStringList files = utils::stringListFromVector(fileNames);
            m_file_panel->NewFile(files);
        };

    lua["navi"]["api"]["search"] = [this](const std::string &searchTerm) noexcept {
        m_file_panel->Search(QString::fromStdString(searchTerm));
    };

    lua["navi"]["api"]["search_next"] = [this]() noexcept { m_file_panel->SearchNext(); };

    lua["navi"]["api"]["search_prev"] = [this]() noexcept { m_file_panel->SearchPrev(); };

    lua["navi"]["api"]["sort_name"] = [this]() noexcept {};

    lua["navi"]["api"]["highlight"] = [this](const std::string &item_path) noexcept {
        m_file_panel->HighlightItem(QString::fromStdString(item_path));
    };

    lua["navi"]["api"]["mount_drive"] = [this](const std::string &path) noexcept {
        MountDrive(QString::fromStdString(path));
    };

    lua["navi"]["api"]["unmount_drive"] = [this](const std::string &path) noexcept {
        UnmountDrive(QString::fromStdString(path));
    };

    lua["navi"]["api"]["list_runtime_paths"] = [this]() noexcept {
        return Lua__list_runtime_paths();
    };

    lua["navi"]["keymap"] = lua.create_table();

    lua["navi"]["keymap"]["set"] = sol::overload(
        [this](const sol::table &table) noexcept {
            Lua__keymap_set(table);
        },

        [this](const std::string &key,
               const std::string &command,
               const std::string &desc) noexcept {
            Lua__keymap_set(key, command, desc);
        });

    // UI API

    lua["navi"]["ui"] = lua.create_table();

    lua["navi"]["ui"]["menubar"] = lua.create_table();

    lua["navi"]["ui"]["menubar"]["toggle"] = [this](const bool &state) noexcept {
        ToggleMenuBar(state);
    };

    lua["navi"]["ui"]["menubar"]["toggle"] = [this]() noexcept { ToggleMenuBar(); };

    lua["navi"]["ui"]["menubar"]["add_menu"] =
        [this](const sol::table &menu_item) noexcept {
            Lua__AddMenu(menu_item);
        };

    lua["navi"]["ui"]["statusbar"] = lua.create_table();

    lua["navi"]["ui"]["statusbar"]["toggle"] = [this](const bool &state) noexcept {
        ToggleStatusBar(state);
    };

    lua["navi"]["ui"]["statusbar"]["toggle"] = [this]() noexcept { ToggleStatusBar(); };

    lua["navi"]["ui"]["statusbar"]["add_module"] =
        [this](const Statusbar::Module &module) noexcept {
            m_statusbar->Lua__AddModule(module);
        };

    lua["navi"]["ui"]["statusbar"]["set_modules"] =
        [this](const sol::table &table) noexcept {
            m_statusbar->Lua__SetModules(table);
        };

    lua["navi"]["ui"]["statusbar"]["insert_module"] = [this](const Statusbar::Module &module,
                                                             const uint32_t &index) noexcept {
            m_statusbar->Lua__InsertModule(module, index);
        };

    lua["navi"]["ui"]["statusbar"]["create_module"] =
        [this](const std::string &name, const sol::table &table) noexcept {
            return m_statusbar->Lua__CreateModule(name, table);
        };

    lua["navi"]["ui"]["statusbar"]["set_module_text"] =
        [this](const std::string &name, const sol::object &obj) noexcept {
            if (obj.is<std::string>())
                m_statusbar->Lua__UpdateModuleText(name, obj.as<std::string>());
            else if (obj.is<sol::function>()) {
                auto func = obj.as<sol::function>();
                sol::object value = func();
                if (value.is<std::string>())
                    m_statusbar->Lua__UpdateModuleText(name, value.as<std::string>());
            }
        };

    lua["navi"]["ui"]["pathbar"] = [this](const bool &state) noexcept {
        TogglePathWidget(state);
    };

    lua["navi"]["ui"]["pathbar"] = [this]() noexcept { TogglePathWidget(); };

    lua["navi"]["ui"]["preview_panel"] = [this](const bool &state) noexcept {
        TogglePreviewPanel(state);
    };

    lua["navi"]["ui"]["preview_panel"] = [this]() noexcept { TogglePreviewPanel(); };

    lua["navi"]["ui"]["messages"] = [this]() noexcept { ToggleMessagesBuffer(); };

    lua["navi"]["ui"]["messages"] = [this](const bool &state) noexcept {
        ToggleMessagesBuffer(state);
    };

    lua["navi"]["ui"]["shortcuts"] = [this](const bool &state) noexcept {
        ToggleShortcutsBuffer(state);
    };

    lua["navi"]["ui"]["shortcuts"] = [this]() noexcept { ToggleShortcutsBuffer(); };

    lua["navi"]["ui"]["marks"] = [this](const bool &state) noexcept {
        ToggleMarksBuffer(state);
    };

    lua["navi"]["ui"]["marks"] = [this]() noexcept { ToggleMarksBuffer(); };

    lua["navi"]["ui"]["context_menu"] = lua.create_table();

    lua["navi"]["ui"]["context_menu"]["create"] = [this](const sol::table &table) noexcept {
        Lua__AddContextMenu(table);
    };

    lua["navi"]["ui"]["toolbar"] = lua.create_table();

    lua["navi"]["ui"]["toolbar"]["add_button"] = [this](const Navi::ToolbarItem &tb_item) noexcept {
        Lua__AddToolbarButton(tb_item);
    };

    lua["navi"]["ui"]["toolbar"]["create_button"] = [this](const std::string &name, const sol::table &table) noexcept {
        return Lua__CreateToolbarButton(name, table);
    };

    lua["navi"]["ui"]["toolbar"]["set_items"] = [this](const sol::table &table) noexcept {
        Lua__SetToolbarItems(table);
    };

    // IO API
    lua["navi"]["io"] = lua.create_table();

    lua["navi"]["io"]["msg"] = [this](const std::string &message,
                                      const MessageType &type) noexcept {
            m_statusbar->Message(QString::fromStdString(message), type);
        };

    lua["navi"]["io"]["msgtype"] =
        lua.create_table_with("info", MessageType::INFO, "warn",
                              MessageType::WARNING, "error", MessageType::ERROR);

    lua["navi"]["io"]["input"] = [this](const std::string &prompt,
                                        const std::string &def_text,
                                        const std::string &selection_text) noexcept {
            return this->Lua__Input(prompt, def_text, selection_text);
        };

    // SHELL API

    lua["navi"]["shell"] = lua.create_table();
    lua["navi"]["shell"]["execute"] = [this](const std::string &command) noexcept {
        ShellCommandAsync(QString::fromStdString(command));
    };
}
