#include "Navi.hpp"
#include "API.hpp"

void init_lua_api(sol::state &lua) noexcept {

    lua.open_libraries(sol::lib::base,
                       sol::lib::jit,
                       sol::lib::package,
                       sol::lib::package,
                       sol::lib::string,
                       sol::lib::table,
                       sol::lib::coroutine,
                       sol::lib::io,
                       sol::lib::os);

    update_lua_package_path(lua);

    lua.new_usertype<Navi::ToolbarItem>("ToolbarItem",
                                        "name", &Navi::ToolbarItem::name,
                                        "icon", &Navi::ToolbarItem::icon,
                                        "tooltip", &Navi::ToolbarItem::tooltip,
                                        "position", &Navi::ToolbarItem::position,
                                        "action", &Navi::ToolbarItem::action,
                                        "label", &Navi::ToolbarItem::label);

    lua.new_usertype<Statusbar::Module>("StatusbarModule",
                                        "name", &Statusbar::Module::name,
                                        "options", &Statusbar::Module::options);

    // Bind MyClass to Lua
    lua.new_usertype<Navi>("Navi",
                           sol::no_constructor,
                           // LAYOUT
                           "load_layout", &Navi::loadLayout,
                           "save_layout", &Navi::saveLayout,
                           "delete_layout", &Navi::deleteLayout,
                           "list_layout", &Navi::listLayout,

                           "set_hidden_files_visible", sol::resolve<void(const bool &)>(&Navi::ToggleHiddenFiles),
                           "get_hidden_files_visible", &Navi::get_hidden_files_visible,
                           "get_terminal", &Navi::Get_terminal,
                           "set_terminal", &Navi::Set_terminal,
                           "set_keymap", &Navi::Lua__keymap_set,
                           "set_keymap_for_function", &Navi::Lua__keymap_set_for_function,
                           "set_copy_path_separator", sol::resolve<void(const std::string &)>(&Navi::Set_copy_path_separator),
                           "get_copy_path_separator", &Navi::Get_copy_path_separator,
                           "set_default_dir", sol::resolve<void(const std::string &)>(&Navi::Set_default_directory),
                           "get_default_dir", &Navi::Get_default_directory,
                           "get_cycle", &Navi::get_cycle,
                           "set_cycle", &Navi::set_cycle,
                           "screenshot", &Navi::screenshot,

                           // Headers
                           "set_header_visible", &Navi::set_header_visible,
                           "get_header_visible", &Navi::get_header_visible,
                           "set_header_columns", &Navi::set_header_columns,
                           "get_header_columns", &Navi::get_header_columns,
                           "set_header_stylesheet", &Navi::set_header_stylesheet,
                           "get_header_stylesheet", &Navi::get_header_stylesheet,

                           // Vertical header
                           "set_vheader_visible", &Navi::set_vheader_visible,
                           "get_vheader_visible", &Navi::get_vheader_visible,
                           "vheader_toggle", &Navi::vheader_toggle,
                           "set_vheader_border", &Navi::set_vheader_border,
                           "get_vheader_border", &Navi::get_vheader_border,
                           "set_vheader_props", &Navi::set_vheader_props,
                           "get_vheader_props", &Navi::get_vheader_props,
                           "set_vheader_foreground", &Navi::set_vheader_foreground,
                           "get_vheader_foreground", &Navi::get_vheader_foreground,
                           "set_vheader_background", &Navi::set_vheader_background,
                           "get_vheader_background", &Navi::get_vheader_background,
                           "set_vheader_stylesheet", &Navi::set_vheader_stylesheet,
                           "get_vheader_stylesheet", &Navi::get_vheader_stylesheet,

                           // Bulk Rename
                           "set_bulk_rename_editor", &Navi::set_bulk_rename_editor,
                           "get_bulk_rename_editor", &Navi::get_bulk_rename_editor,
                           "set_bulk_rename_with_terminal", &Navi::set_bulk_rename_with_terminal,
                           "get_bulk_rename_with_terminal", &Navi::get_bulk_rename_with_terminal,
                           "set_bulk_rename_file_threshold", &Navi::set_bulk_rename_file_threshold,
                           "get_bulk_rename_file_threshold", &Navi::get_bulk_rename_file_threshold,

                           // Inputbar font
                           "get_inputbar_font", &Navi::get_inputbar_font,
                           "set_inputbar_font", &Navi::set_inputbar_font,
                           "get_inputbar_background", &Navi::get_inputbar_background,
                           "set_inputbar_background", &Navi::set_inputbar_background,
                           "get_inputbar_foreground", &Navi::get_inputbar_foreground,
                           "set_inputbar_foreground", &Navi::set_inputbar_foreground,
                           "set_inputbar_font_size", &Navi::set_inputbar_font_size,
                           "get_inputbar_font_size", &Navi::get_inputbar_font_size,
                           "set_inputbar_props", &Navi::set_inputbar_props,
                           "get_inputbar_props", &Navi::get_inputbar_props,
                           "get_inputbar_visible", &Navi::get_inputbar_visible,

                           // Completion Popup
                           "set_completion_font", &Navi::set_completion_font,
                           "get_completion_font", &Navi::get_completion_font,
                           "set_completion_font_size", &Navi::set_completion_font_size,
                           "get_completion_font_size", &Navi::get_completion_font_size,
                           "set_completion_grid", &Navi::set_completion_grid,
                           "get_completion_grid", &Navi::get_completion_grid,
                           "set_completion_line_numbers", &Navi::set_completion_line_numbers,
                           "get_completion_line_numbers", &Navi::get_completion_line_numbers,
                           "set_completion_count_visible", &Navi::set_completion_count_visible,
                           "get_completion_count_visible", &Navi::get_completion_count_visible,
                           "set_completion_props", &Navi::set_completion_props,
                           "get_completion_props", &Navi::get_completion_props,

                           // Statusbar
                           "create_statusbar_module", &Navi::create_statusbar_module,
                           "toggle_statusbar", sol::resolve<void()>(&Navi::ToggleStatusBar),
                           "set_statusbar_modules", &Navi::set_statusbar_modules,
                           "set_statusbar_module_text", &Navi::set_statusbar_module_text,
                           "set_statusbar_visible", &Navi::set_statusbar_visible,
                           "get_statusbar_visible", &Navi::get_statusbar_visible,
                           "set_statusbar_message_warn_fg", &Navi::set_statusbar_message_warn_fg,
                           "get_statusbar_message_warn_fg", &Navi::get_statusbar_message_warn_fg,
                           "set_statusbar_message_info_fg", &Navi::set_statusbar_message_info_fg,
                           "get_statusbar_message_info_fg", &Navi::get_statusbar_message_info_fg,
                           "set_statusbar_message_error_fg", &Navi::set_statusbar_message_error_fg,
                           "get_statusbar_message_error_fg", &Navi::get_statusbar_message_error_fg,

                           // Pathbar
                           "toggle_pathbar", sol::resolve<void()>(&Navi::ToggleStatusBar),
                           "focus_pathbar", &Navi::FocusPath,
                           "set_pathbar_font", &Navi::set_pathbar_font,
                           "get_pathbar_font", &Navi::get_pathbar_font,
                           "set_pathbar_visible", &Navi::set_pathbar_visible,
                           "get_pathbar_visible", &Navi::get_pathbar_visible,
                           "set_pathbar_bold", &Navi::set_pathbar_bold,
                           "get_pathbar_bold", &Navi::get_pathbar_bold,
                           "set_pathbar_italic", &Navi::set_pathbar_italic,
                           "get_pathbar_italic", &Navi::get_pathbar_italic,
                           "set_pathbar_underline", &Navi::set_pathbar_underline,
                           "get_pathbar_underline", &Navi::get_pathbar_underline,
                           "set_pathbar_font_size", &Navi::set_pathbar_font_size,
                           "get_pathbar_font_size", &Navi::get_pathbar_font_size,
                           "set_pathbar_foreground", &Navi::set_pathbar_foreground,
                           "get_pathbar_foreground", &Navi::get_pathbar_foreground,
                           "set_pathbar_background", &Navi::set_pathbar_background,
                           "get_pathbar_background", &Navi::get_pathbar_background,
                           "set_pathbar_props", &Navi::set_pathbar_props,
                           "get_pathbar_props", &Navi::get_pathbar_props,

                           // Toolbar
                           "toggle_toolbar", &Navi::toggle_toolbar,
                           "create_toolbar_button", &Navi::Lua__CreateToolbarButton,
                           "add_toolbar_button", sol::resolve<void(const Navi::ToolbarItem&)>(&Navi::Lua__AddToolbarButton),
                           "set_toolbar_visible", &Navi::set_toolbar_visible,
                           "get_toolbar_visible", &Navi::get_toolbar_visible,
                           "set_toolbar_icons_only", &Navi::set_toolbar_icons_only,
                           "get_toolbar_icons_only", &Navi::get_toolbar_icons_only,
                           "set_toolbar_layout", &Navi::set_toolbar_layout,
                           /*"get_toolbar_layout", &Navi::get_toolbar_layout,*/
                           "set_toolbar_props", &Navi::set_toolbar_props,

                           // Menubar
                           "toggle_menubar", sol::resolve<void(void)>(&Navi::ToggleMenuBar),
                           "add_menubar_menu", &Navi::Lua__AddMenu,
                           "set_menubar_visible", &Navi::set_menubar_visible,
                           "get_menubar_visible", &Navi::get_menubar_visible,
                           "set_menubar_icons", &Navi::set_menubar_icons,
                           "get_menubar_icons", &Navi::get_menubar_icons,
                           "set_menubar_props", &Navi::set_menubar_props,
                           "get_menubar_props", &Navi::get_menubar_props,

                           // Preview Panel
                           "toggle_preview_panel", sol::resolve<void(void)>(&Navi::TogglePreviewPanel),
                           "set_preview_panel_visible", &Navi::set_preview_panel_visible,
                           "get_preview_panel_visible", &Navi::get_preview_panel_visible,
                           "set_preview_panel_fraction", &Navi::Set_preview_pane_fraction,
                           "get_preview_panel_fraction", &Navi::Preview_pane_fraction,
                           "set_preview_panel_max_file_size", &Navi::set_preview_panel_max_file_size,
                           "get_preview_panel_max_file_size", &Navi::get_preview_panel_max_file_size,
                           "set_preview_panel_image_dimension", &Navi::set_preview_panel_image_dimension,
                           "get_preview_panel_image_dimension", &Navi::get_preview_panel_image_dimension,
                           "set_preview_panel_num_read_lines", &Navi::set_preview_panel_read_num_lines,
                           "get_preview_panel_num_read_lines", &Navi::get_preview_panel_read_num_lines,
                           "set_preview_panel_props", &Navi::set_preview_panel_props,
                           "get_preview_panel_props", &Navi::get_preview_panel_props,
                           "set_preview_panel_image_rotation", &Navi::set_preview_image_rotation,

                           // API
                           "copy_path", &Navi::copy_path,
                           "list_runtime_paths", &Navi::list_runtime_paths,
                           "create_user_command", &Navi::Lua__register_user_function,
                           "remove_user_command", &Navi::Lua__unregister_user_function,
                           "execute_lua_code", &Navi::execute_lua_code,
                           "count", &Navi::count_item,
                           "search", sol::resolve<void(const std::string&)>(&Navi::Search),
                           "search_next", &Navi::SearchNext,
                           "search_prev", &Navi::SearchPrev,
                           "next_item", &Navi::NextItem,
                           "prev_item", &Navi::PrevItem,
                           "first_item", &Navi::GotoFirstItem,
                           "last_item", &Navi::GotoLastItem,
                           "middle_item", &Navi::GotoMiddleItem,
                           "select_item", &Navi::SelectItem,
                           "item_name", &Navi::current_item_name,
                           "item_property", &Navi::item_property,
                           "parent_directory", &Navi::UpDirectory,
                           "mount_drive", sol::resolve<bool(const std::string&)>(&Navi::mount_drive),
                           "unmount_drive", sol::resolve<bool(const std::string&)>(&Navi::unmount_drive),
                           "create_file", &Navi::new_file,
                           "create_directory", &Navi::new_folder,
                           "trash", &Navi::TrashItem,
                           "trash_dwim", &Navi::TrashDWIM,
                           "copy", &Navi::CopyItem,
                           "copy_dwim", &Navi::CopyDWIM,
                           "cut", &Navi::CutItem,
                           "cut_dwim", &Navi::CutDWIM,
                           "has_marks_local", &Navi::has_marks_local,
                           "has_marks_global", &Navi::has_marks_global,
                           "global_marks", &Navi::global_marks,
                           "local_marks", &Navi::local_marks,
                           "local_marks_count", &Navi::local_marks_count,
                           "global_marks_count", &Navi::global_marks_count,
                           "highlight", &Navi::highlight,
                           "cd", &Navi::change_directory,
                           "pwd", &Navi::working_directory,
                           "filter", &Navi::filter,
                           "reset_filter", &Navi::ResetFilter,
                           "mark", &Navi::MarkItem,
                           "mark_dwim", &Navi::MarkDWIM,
                           "mark_all", &Navi::MarkAllItems,
                           "mark_inverse", &Navi::MarkInverse,
                           "mark_regex", &Navi::mark_regex,
                           "unmark", &Navi::UnmarkItem,
                           "unmark_dwim", &Navi::UnmarkDWIM,
                           "unmark_regex", &Navi::unmark_regex,
                           "toggle_mark", &Navi::ToggleMarkItem,
                           "toggle_mark_dwim", &Navi::ToggleMarkDWIM,
                           "rename", &Navi::RenameItem,
                           "rename_dwim", &Navi::RenameDWIM,
                           "delete", &Navi::DeleteItem,
                           "delete_dwim", &Navi::DeleteDWIM,
                           "chmod", &Navi::ChmodItem,
                           "chmod_dwim", &Navi::ChmodDWIM,
                           "has_selection", &Navi::has_selection,
                           "message", &Navi::message,
                           "input", &Navi::input,
                           "input_items", &Navi::input_items,
                           "execute", &Navi::execute_shell_command,

                           // Statusbar
                           "set_statusbar_font", &Navi::set_statusbar_font,
                           "get_statusbar_font", &Navi::get_statusbar_font,
                           "set_statusbar_font_size", &Navi::set_statusbar_font_size,
                           "get_statusbar_font_size", &Navi::get_statusbar_font_size,
                           "set_statusbar_background", &Navi::set_statusbar_background,
                           "get_statusbar_background", &Navi::get_statusbar_background,
                           "set_statusbar_modules", &Navi::set_statusbar_modules,
                           "add_statusbar_module", &Navi::add_statusbar_module,
                           /*"get_statusbar_modules", &Navi::get_statusbar_modules,*/

                           // Visual line mode
                           "set_visual_line_mode_text", &Navi::set_visual_line_mode_text,
                           "set_visual_line_mode_background", &Navi::set_visual_line_mode_background,
                           "set_visual_line_mode_foreground", &Navi::set_visual_line_mode_foreground,
                           "set_visual_line_mode_italic", &Navi::set_visual_line_mode_italic,
                           "set_visual_line_mode_bold", &Navi::set_visual_line_mode_bold,
                           "set_visual_line_mode_padding", &Navi::set_visual_line_mode_padding,

                           // Hooks
                           "add_hook", &Navi::add_hook,
                           "trigger_hook", &Navi::trigger_hook,
                           "clear_functions_for_hook", &Navi::clear_functions_for_hook,

                           // Bookmarks
                           "add_bookmark", &Navi::add_bookmark,
                           "remove_bookmark", &Navi::remove_bookmark,

                           // Bookmarks Options
                           "set_bookmark_auto_save", &Navi::set_bookmark_auto_save,
                           "get_bookmark_auto_save", &Navi::get_bookmark_auto_save,

                           // File Panel
                           "set_file_panel_icons", &Navi::set_file_panel_icons,
                           "get_file_panel_icons", &Navi::get_file_panel_icons,
                           "set_file_panel_font", &Navi::set_file_panel_font,
                           "get_file_panel_font", &Navi::get_file_panel_font,
                           "set_file_panel_font_size", &Navi::set_file_panel_font_size,
                           "get_file_panel_font_size", &Navi::get_file_panel_font_size,
                           "set_file_panel_props", &Navi::set_file_panel_props,
                           "get_file_panel_props", &Navi::get_file_panel_props,
                           "set_file_panel_grid", &Navi::set_file_panel_grid,
                           "get_file_panel_grid", &Navi::get_file_panel_grid,
                           "set_file_panel_grid_style", &Navi::set_file_panel_grid_style,
                           "get_file_panel_grid_style", &Navi::get_file_panel_grid_style,

                           // Symlink
                           "set_symlink_visible", &Navi::set_symlink_visible,
                           "get_symlink_visible", &Navi::get_symlink_visible,
                           "set_symlink_foreground", &Navi::set_symlink_foreground,
                           "get_symlink_foreground", &Navi::get_symlink_foreground,
                           "set_symlink_background", &Navi::set_symlink_background,
                           "get_symlink_background", &Navi::get_symlink_background,
                           "set_symlink_italic", &Navi::set_symlink_italic,
                           "get_symlink_italic", &Navi::get_symlink_italic,
                           "set_symlink_bold", &Navi::set_symlink_bold,
                           "get_symlink_bold", &Navi::get_symlink_bold,
                           "set_symlink_underline", &Navi::set_symlink_underline,
                           "get_symlink_underline", &Navi::get_symlink_underline,
                           "set_symlink_font", &Navi::set_symlink_font,
                           "get_symlink_font", &Navi::get_symlink_font,
                           "set_symlink_font_size", &Navi::set_symlink_font_size,
                           "get_symlink_font_size", &Navi::get_symlink_font_size,
                           "set_symlink_props", &Navi::set_symlink_props,
                           "get_symlink_props", &Navi::get_symlink_props,
                           "set_symlink_separator", &Navi::set_symlink_separator,
                           "get_symlink_separator", &Navi::get_symlink_separator,

                           // cursor

                           "get_cursor_foreground", &Navi::get_cursor_foreground,
                           "set_cursor_foreground", &Navi::set_cursor_foreground,
                           "get_cursor_background", &Navi::get_cursor_background,
                           "set_cursor_background", &Navi::set_cursor_background,
                           "get_cursor_italic", &Navi::get_cursor_italic,
                           "set_cursor_italic", &Navi::set_cursor_italic,
                           "get_cursor_bold", &Navi::get_cursor_bold,
                           "set_cursor_bold", &Navi::set_cursor_bold,
                           "get_cursor_underline", &Navi::get_cursor_underline,
                           "set_cursor_underline", &Navi::set_cursor_underline,
                           "get_cursor_font", &Navi::get_cursor_font,
                           "set_cursor_font", &Navi::set_cursor_font,
                           "get_cursor_props", &Navi::get_cursor_props,
                           "set_cursor_props", &Navi::set_cursor_props

                           );

    lua["_navi"] = &Navi::getInstance();
}

void update_lua_package_path(sol::state &lua) noexcept {
    QStringList visitedDirs;  // To track directories already added to package.path

    lua.script(R"(
               package = package or {}
               package.path = package.path or ""
               )");

    auto iterateDir = [&](const QString &folder) {
        // Create a QDirIterator to iterate through all subdirectories and files
        QDirIterator it(folder, QStringList() << "*.lua", QDir::Files, QDirIterator::Subdirectories);

        while (it.hasNext()) {
            QString filePath = it.next();
            QString parentDir = QFileInfo(filePath).absolutePath();  // Get the parent directory

            // Add to package.path only if this directory hasn't been added yet
            if (!visitedDirs.contains(parentDir)) {
                /*m_runtime_path.insert(parentDir);*/
                lua.script("package.path = package.path .. ';" + parentDir.toStdString() + "/?.lua'");
                visitedDirs.append(parentDir);  // Mark the directory as visited
            }
        }
    };

    iterateDir(CONFIG_DIR_PATH);
    iterateDir(QString("/usr/share/%1").arg(APP_NAME));
    iterateDir(QString("~/.local/share/%1").arg(APP_NAME));
}
