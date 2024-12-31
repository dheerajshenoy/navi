#include "Navi.hpp"
#include "API.hpp"

void init_lua_api(sol::state &lua) noexcept {

    lua.open_libraries(sol::lib::base, sol::lib::jit,
                       sol::lib::package, sol::lib::package,
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
                           sol::constructors<Navi>(),
                           "get_terminal", &Navi::Get_terminal,
                           "set_terminal", &Navi::Set_terminal,
                           "set_keymap", &Navi::Lua__keymap_set,
                           "set_copy_path_separator", sol::resolve<void(const std::string &)>(&Navi::Set_copy_path_separator),
                           "get_copy_path_separator", &Navi::Get_copy_path_separator,
                           "set_default_dir", sol::resolve<void(const std::string &)>(&Navi::Set_default_directory),
                           "get_default_dir", &Navi::Get_default_directory,
                           "get_cycle", &Navi::get_cycle,
                           "set_cycle", &Navi::set_cycle,

                           // Headers
                           "set_header_visible", &Navi::set_header_visible,
                           "get_header_visible", &Navi::get_header_visible,
                           "set_header_columns", &Navi::set_header_columns,
                           "get_header_columns", &Navi::get_header_columns,


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

                           // Statusbar
                           "create_statusbar_module", &Navi::create_statusbar_module,
                           "toggle_statusbar", sol::resolve<void()>(&Navi::ToggleStatusBar),
                           "set_statusbar_modules", &Navi::set_statusbar_modules,
                           "set_statusbar_module_text", &Navi::set_statusbar_module_text,
                           "set_statusbar_visible", &Navi::set_statusbar_visible,
                           "get_statusbar_visible", &Navi::get_statusbar_visible,

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
                           "set_pathbar_font_size", &Navi::set_pathbar_font_size,
                           "get_pathbar_font_size", &Navi::get_pathbar_font_size,

                           // Toolbar
                           "toggle_toolbar", &Navi::toggle_toolbar,
                           "create_toolbar_button", &Navi::Lua__CreateToolbarButton,
                           "add_toolbar_button", sol::resolve<void(const Navi::ToolbarItem&)>(&Navi::Lua__AddToolbarButton),
                           /*"set_toolbar_items", &Navi::Lua__SetToolbarItems,*/
                           "set_toolbar_visible", &Navi::set_toolbar_visible,
                           "get_toolbar_visible", &Navi::get_toolbar_visible,
                           "set_toolbar_icons_only", &Navi::set_toolbar_icons_only,
                           "get_toolbar_icons_only", &Navi::get_toolbar_icons_only,
                           "set_toolbar_layout", &Navi::Set_toolbar_layout,
                           "get_toolbar_layout", &Navi::get_toolbar_layout,

                           // Menubar
                           "toggle_menubar", sol::resolve<void(void)>(&Navi::ToggleMenuBar),
                           "add_menubar_menu", &Navi::Lua__AddMenu,
                           "set_menubar_visible", &Navi::set_menubar_visible,
                           "get_menubar_visible", &Navi::get_menubar_visible,
                           "set_menubar_icons", &Navi::set_menubar_icons,
                           "get_menubar_icons", &Navi::get_menubar_icons,

                           // Preview Panel
                           "toggle_preview_panel", sol::resolve<void(void)>(&Navi::TogglePreviewPanel),
                           "set_preview_panel_visible", &Navi::set_preview_panel_visible,
                           "get_preview_panel_visible", &Navi::get_preview_panel_visible,
                           "set_preview_panel_fraction", &Navi::Set_preview_pane_fraction,
                           "get_preview_panel_fraction", &Navi::Preview_pane_fraction,
                           "set_preview_panel_max_file_size", &Navi::set_preview_Panel_max_file_size,
                           "get_preview_panel_max_file_size", &Navi::get_preview_Panel_max_file_size,
                           "set_preview_panel_image_dimension", &Navi::set_preview_panel_image_dimension,
                           "get_preview_panel_image_dimension", &Navi::get_preview_panel_image_dimension,
                           "set_preview_panel_num_read_lines", &Navi::set_preview_panel_read_num_lines,
                           "get_preview_panel_num_read_lines", &Navi::get_preview_panel_read_num_lines,
                           "set_preview_panel_props", &Navi::set_preview_panel_props,
                           "get_preview_panel_props", &Navi::get_preview_panel_props,

                           // API
                           "list_runtime_paths", &Navi::list_runtime_paths,
                           "register_function", &Navi::Lua__register_lua_function,
                           "unregister_function", &Navi::Lua__unregister_lua_function,
                           "list_registered_functions", &Navi::Lua__registered_lua_functions,
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

                           "set_file_panel_icons", &Navi::set_file_panel_icons,
                           "get_file_panel_icons", &Navi::get_file_panel_icons,

                           "set_file_panel_font", &Navi::set_file_panel_font,
                           "get_file_panel_font", &Navi::get_file_panel_font,

                           "set_file_panel_font_size", &Navi::set_file_panel_font_size,
                           "get_file_panel_font_size", &Navi::get_file_panel_font_size

                           );


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
    iterateDir("/usr/share/navi");
    iterateDir("~/.local/share/navi");
}
