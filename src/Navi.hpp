#pragma once

#include "utils.hpp"

#define UNUSED(x) (void) x
#include "Globals.hpp"

#include <unistd.h>
#include <QIcon>
#include <QFuture>
#include <QFutureWatcher>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHash>
#include <QInputDialog>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QShortcut>
#include <QSplitter>
#include <QStringListModel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QDebug>
#include <QStringView>
#include <QActionGroup>
#include <QEvent>
#include <QMessageBox>
#include <QInputDialog>
#include "InputDialog.hpp"
#include <QKeyEvent>
#include <QtConcurrent/QtConcurrent>
#include <QPromise>
#include <QFuture>
#include <QThread>
#include <QClipboard>
#include <QToolBar>
#include <QSet>
#include <QHash>
#include <unordered_map>

// Local includes
#include "FilePathWidget.hpp"
#include "Inputbar.hpp"
#include "MessagesBuffer.hpp"
#include "FilePanel.hpp"
#include "PreviewPanel.hpp"
#include "Statusbar.hpp"
#include "MarksBuffer.hpp"
#include "BookmarkManager.hpp"
#include "BookmarkWidget.hpp"
#include "ShortcutsWidget.hpp"
#include "TabBarWidget.hpp"
#include "sol/sol.hpp"
#include "argparse.hpp"
#include "DriveWidget.hpp"
#include "StorageDevice.hpp"
#include "Task.hpp"
#include "TaskManager.hpp"
#include "TasksWidget.hpp"
#include "RegisterWidget.hpp"
#include "HookManager.hpp"
#include "FolderPropertyWidget.hpp"
#include "AboutWidget.hpp"

class Menubar : public QMenuBar {
    Q_OBJECT

    signals:
    void visibilityChanged(const bool &state);

public:
    explicit Menubar(QWidget *parent = nullptr) : QMenuBar(parent) {}

    void hide() noexcept {
        emit visibilityChanged(false);
        QMenuBar::hide();
    }

    void show() noexcept {
        emit visibilityChanged(true);
        QMenuBar::show();
    }
};

class Navi : public QMainWindow {

public:
    Navi(QWidget *parent = nullptr);
    ~Navi();

    inline void set_lua_state(sol::state &lua) noexcept {
        m_lua = &lua;
    }

    inline FilePanel* file_panel() noexcept { return m_file_panel; }
    inline Inputbar* get_inputbar() const noexcept { return m_inputbar; }
    void initThings() noexcept;
    void SelectAllItems() noexcept;
    void SelectInverse() noexcept;
    // Interactive Functions
    void ExecuteExtendedCommand() noexcept;
    void NewFolder(const QStringList &folders = {}) noexcept;
    void new_folder(const std::vector<std::string> &folders = {}) noexcept;
    void NewFile(const QStringList &files = {}) noexcept;
    void new_file(const std::vector<std::string> &files = {}) noexcept;
    void change_directory(const std::string &path) noexcept;

    std::string working_directory() const noexcept {
        return m_file_panel->getCurrentDir().toStdString();
    }

    QString GetInput(const QString &prompt,
                     const QString &title,
                     const QString &default_text) noexcept;

    QString GetInput(const QString &prompt,
                     const QString &title,
                     const QStringList &choice,
                     const int &default_choice) noexcept;

    void PasteItems() noexcept;
    void ShowHelp() noexcept;
    void ToggleMenuBar(const bool &state) noexcept;
    void ToggleMenuBar() noexcept;

    void Filter(const QString &filter_string = QString()) noexcept;
    void filter(const std::string &filter_string = std::string()) noexcept;
    void ResetFilter() noexcept;
    void LogMessage(const QString &message, const MessageType &type = MessageType::INFO) noexcept;
    inline void FocusPath() noexcept { m_file_path_widget->setFocus(); }
    void AddBookmark(const QStringList &bookmarkName = QStringList()) noexcept;
    void RemoveBookmark(const QStringList &bookmarkName = QStringList()) noexcept;
    void SortDescending(const bool &state) noexcept;
    void SortAscending(const bool &state) noexcept;

    std::map<std::string, std::string> Get_bulk_rename() const noexcept {
        std::map<std::string, std::string> map;
        map["file_threshold"] = m_file_panel->Bulk_rename_threshold();
        map["editor"] = m_file_panel->Bulk_rename_editor();
        map["with_terminal"] = m_file_panel->Is_bulk_rename_with_terminal();
        return map;
    }

    inline void set_pathbar_font(const std::string &font) noexcept {
        m_file_path_widget->setFont(QString::fromStdString(font));
    }

    inline std::string get_pathbar_font() const noexcept {
        return m_file_path_widget->font().family().toStdString();
    }

    inline int get_pathbar_font_size() const noexcept {
        return m_file_path_widget->get_font_size();
    }

    inline void set_pathbar_font_size(const int &size) noexcept {
        m_file_path_widget->set_font_size(size);
    }

    inline void message(const std::string &msg, const MessageType &type) noexcept {
        m_statusbar->Message(QString::fromStdString(msg), type);
    }

    inline std::string input(const std::string &prompt,
                             const std::string &title,
                             const std::string &def) noexcept {

        QString result = GetInput(QString::fromStdString(prompt),
                                  QString::fromStdString(title),
                                  QString::fromStdString(def));
        return result.toStdString();
    }

    inline std::string input_items(const std::string &prompt,
                                   const std::string &title,
                                   const std::vector<std::string> &items,
                                   const int &default_choice) noexcept {

        return GetInput(QString::fromStdString(prompt),
                        QString::fromStdString(title),
                        utils::stringListFromVector(items), default_choice - 1).toStdString();
    }

    inline void Set_bulk_rename(const sol::table &table) noexcept {
        auto editor = table["editor"].get<std::string>();
        auto threshold = table["file_threshold"].get<int>();
        auto state = table["with_terminal"].get<bool>();
        m_file_panel->SetBulkRenameEditor(QString::fromStdString(editor));
        m_file_panel->SetBulkRenameThreshold(threshold);
        m_file_panel->SetBulkRenameWithTerminal(state);
    }

    struct MenuItem {
        std::string label;
        std::function<void()> action;
        std::vector<MenuItem> submenu;

        inline void execute() noexcept {
            if (action) {
                action();
            }
        }
    };

    struct ToolbarItem {
        std::string name;
        std::string label;
        std::string icon;
        std::string tooltip;
        int position = -1;
        std::function<void()> action;

        inline void execute() noexcept {
            if (action) {
                action();
            }

        }
    };

    void EditBookmarkName(const QStringList &bookmarkName) noexcept;
    void EditBookmarkFile(const QStringList &bookmarkName) noexcept;
    void LoadBookmarkFile(const QStringList &bookmarkFilePath) noexcept;
    void GoBookmark(const QString &bookmarkName = QString()) noexcept;
    void SaveBookmarkFile() noexcept;
    void SortByName(const bool &reverse = false) noexcept;
    void SortByDate(const bool &reverse = false) noexcept;
    void SortBySize(const bool &reverse = false) noexcept;
    void ShellCommandAsync(const QString &command = "") noexcept;
    void Search(const QString & = QString()) noexcept;
    void Search(const std::string & = std::string()) noexcept;
    void SearchRegex() noexcept;
    void ToggleRecordMacro() noexcept;
    void PlayMacro() noexcept;
    void DeleteMacro() noexcept;
    void EditMacro() noexcept;
    void ListMacro() noexcept;
    // void ToggleSyntaxHighlight() noexcept;
    void SearchNext() noexcept;
    void SearchPrev() noexcept;
    void ToggleCycle() noexcept;
    void ToggleHeaders() noexcept;
    void RenameItem() noexcept;
    void RenameItemsGlobal() noexcept;
    void RenameItemsLocal() noexcept;
    void RenameDWIM() noexcept;
    void CopyItem() noexcept;
    void CopyItemsGlobal() noexcept;
    void CopyItemsLocal() noexcept;
    void CopyDWIM() noexcept;
    void CutItem() noexcept;
    void CutItemsGlobal() noexcept;
    void CutItemsLocal() noexcept;
    void CutDWIM() noexcept;
    void DeleteItem() noexcept;
    void DeleteItemsGlobal() noexcept;
    void DeleteItemsLocal() noexcept;
    void DeleteDWIM() noexcept;
    void TrashItem() noexcept;
    void TrashItemsGlobal() noexcept;
    void TrashItemsLocal() noexcept;
    void TrashDWIM() noexcept;
    void MarkRegex() noexcept;
    void mark_regex(const std::string &) noexcept;
    void highlight(const std::string &name) noexcept;
    void MarkItem() noexcept;
    void MarkInverse() noexcept;
    void MarkAllItems() noexcept;
    void MarkDWIM() noexcept;
    void ToggleMarkItem() noexcept;
    void ToggleMarkDWIM() noexcept;
    void UnmarkItem() noexcept;
    void UnmarkItemsLocal() noexcept;
    void UnmarkItemsGlobal() noexcept;
    void UnmarkRegex() noexcept;
    void UnmarkDWIM() noexcept;
    void ForceUpdate() noexcept;
    void ChmodItem() noexcept;
    void ChmodItemsLocal() noexcept;
    void ChmodItemsGlobal() noexcept;
    void ChmodDWIM() noexcept;
    void ShowItemPropertyWidget() noexcept;
    void UpDirectory() noexcept;
    void SelectItem() noexcept;
    void NextItem() noexcept;
    void PrevItem() noexcept;
    void GotoFirstItem() noexcept;
    void GotoLastItem() noexcept;
    void GotoMiddleItem() noexcept;
    void ToggleMouseScroll() noexcept;
    void ToggleVisualLine() noexcept;
    void SpawnProcess(const QString &command,
                      const QStringList &args) noexcept;
    void ToggleDrivesWidget(const bool &state) noexcept;
    void ToggleDrivesWidget() noexcept;
    void TogglePathWidget(const bool &state) noexcept;
    void TogglePathWidget() noexcept;
    void ToggleStatusBar(const bool &state) noexcept;
    void ToggleStatusBar() noexcept;
    void TogglePreviewPanel(const bool &state) noexcept;
    void TogglePreviewPanel() noexcept;
    void ToggleMessagesBuffer(const bool &state) noexcept;
    void ToggleMessagesBuffer() noexcept;
    void ToggleMarksBuffer(const bool &state) noexcept;
    void ToggleMarksBuffer() noexcept;
    void ToggleBookmarksBuffer(const bool &state) noexcept;
    void ToggleBookmarksBuffer() noexcept;
    void ToggleShortcutsBuffer(const bool &state) noexcept;
    void ToggleShortcutsBuffer() noexcept;
    void ToggleHiddenFiles(const bool &state) noexcept;
    void ToggleHiddenFiles() noexcept;
    void ToggleDotDot(const bool &state) noexcept;
    void ToggleDotDot() noexcept;
    void ToggleTasksWidget() noexcept;
    void ToggleTasksWidget(const bool &state) noexcept;
    void ToggleRegisterWidget() noexcept;
    void ToggleRegisterWidget(const bool &state) noexcept;
    void execute_lua_code(const std::string &code) noexcept;
    void MountDrive(const QString &driveName) noexcept;
    void UnmountDrive(const QString &driveName) noexcept;
    void readArgumentParser(argparse::ArgumentParser &parser);
    void GotoItem(const int &num) noexcept;
    void ChangeDirectory(const QString &dir = QString()) noexcept;
    void LaunchNewInstance() noexcept;
    void ShowFolderProperty() noexcept;
    void Exit() noexcept;
    void CopyPath(const QString &separator = QString()) noexcept;
    void FullScreen() noexcept;
    void FullScreen(const bool &state) noexcept;
    void ShowAbout() noexcept;

    void execute_shell_command(const std::string &command,
                               std::vector<std::string> &args) noexcept;
    void unmark_regex(const std::string &term) noexcept;
    bool mount_drive(const std::string &drive_name) noexcept;
    bool unmount_drive(const std::string &drive_name) noexcept;
    // Lua helper functions

    std::string Lua__Input(const std::string &prompt,
                           const std::string &default_value,
                           const std::string &default_selection) noexcept;
    void Lua__Shell(const std::string &command) noexcept;
    void Lua__CreateFolders(const std::vector<std::string> &paths) noexcept;
    void Lua__AddMenu(const sol::table &menu) noexcept;
    void Lua__AddContextMenu(const sol::table &cmenu) noexcept;
    void Lua__AddToolbarButton(const ToolbarItem &item) noexcept;
    void Lua__AddToolbarButton(const sol::object &object) noexcept;
    Navi::MenuItem Lua__parseMenuItem(const sol::table &table) noexcept;
    Navi::ToolbarItem Lua__parseToolbarItem(const sol::table &table) noexcept;
    void Lua__SetToolbarItems(const sol::table &table) noexcept;
    Navi::ToolbarItem Lua__CreateToolbarButton(const std::string &name,
                                               const sol::table &table) noexcept;
    void Lua__keymap_set_from_table(const sol::table &table) noexcept;
    void Lua__keymap_set(const std::string &key,
                         const std::string &command,
                         const std::string &desc) noexcept;
    void Lua__register_user_function(const std::string &name,
                                    const sol::function &func) noexcept;
    void Lua__unregister_user_function(const std::string &name) noexcept;
    /*sol::table Lua__registered_lua_functions(sol::state &lua) noexcept;*/
    void Set_default_directory(const QString &dir) noexcept;
    void Set_default_directory(const std::string &dir) noexcept;
    inline std::string Get_default_directory() noexcept { return m_default_dir.toStdString(); }
    void set_menubar_icons(const bool &state) noexcept;
    double Preview_pane_fraction() noexcept;
    void Set_preview_pane_fraction(const double &fraction) noexcept;

    inline void set_statusbar_background(const std::string &color) noexcept {
        m_statusbar->set_background_color(QString::fromStdString(color));
    }

    inline void add_hook(const std::string &name, const sol::function &func) noexcept {
        m_hook_manager->addHook(name, func);
    }

    inline void trigger_hook(const std::string &hook) noexcept {
        m_hook_manager->triggerHook(hook);
    }

    inline void clear_functions_for_hook(const std::string &hook) noexcept {
        m_hook_manager->clearHookFunctions(hook);
    }

    inline bool add_bookmark(const std::string &bookmark_name, const bool &highlight = false) noexcept {
        return m_bookmark_manager->addBookmark(QString::fromStdString(bookmark_name),
                                        QString::fromStdString(current_item_name()),
                                        highlight);
    }

    inline bool remove_bookmark(const std::string &bookmark_name) noexcept {
        return m_bookmark_manager->removeBookmark(QString::fromStdString(bookmark_name));
    }

    inline std::string get_statusbar_background() noexcept {
        return m_statusbar->get_background_color();
    }
    inline std::string Get_copy_path_separator() noexcept { return m_copy_path_join_str.toStdString(); }


    void Set_copy_path_separator(const QString &sep) noexcept {
        m_copy_path_join_str = sep;
    }

    void Set_copy_path_separator(const std::string &sep) noexcept {
        m_copy_path_join_str = QString::fromStdString(sep);
    }

    inline void Set_terminal(const std::string &term) noexcept {
        m_terminal = QString::fromStdString(term);
        m_file_panel->SetTerminal(m_terminal);
    }

    inline void set_inputbar_font(const std::string &font) noexcept {
        m_inputbar->setFontFamily(QString::fromStdString(font));
    }

    inline std::string get_inputbar_font() noexcept {
        return m_inputbar->font().family().toStdString();
    }

    inline void set_inputbar_background(const std::string &bg) noexcept {
        m_inputbar->setBackground(QString::fromStdString(bg));
    }

    inline std::string get_inputbar_background() noexcept {
        return m_inputbar->Get_background_color();
    }

    inline void set_inputbar_foreground(const std::string &bg) noexcept {
        m_inputbar->setBackground(QString::fromStdString(bg));
    }

    inline std::string get_inputbar_foreground() noexcept {
        return m_inputbar->Get_foreground_color();
    }

    void set_inputbar_props(const sol::table &table) noexcept;

    inline std::string get_statusbar_font() noexcept {
        return m_statusbar->get_font_family();
    }

    inline void set_statusbar_font(const std::string &font) noexcept {
        m_statusbar->set_font_family(QString::fromStdString(font));
    }

    inline void set_statusbar_font_size(const int &size) noexcept {
        m_statusbar->set_font_size(size);
    }

    inline int get_statusbar_font_size() noexcept {
        return m_statusbar->get_font_size();
    }

    inline bool has_marks_local() noexcept {
        return m_file_panel->model()->hasMarksLocal();
    }

    inline bool has_marks_global() noexcept {
        return m_file_panel->model()->hasMarks();
    }

    inline sol::table global_marks(sol::state &lua) noexcept {
        auto files = m_file_panel->model()->getMarkedFiles();
        sol::table table = lua.create_table();
        if (files.isEmpty())
            return table;
        for (int i=0; i < files.size(); i++)
            table[i + 1] = files.at(i);
        return table;
    }
    inline sol::table local_marks(sol::state &lua) noexcept {
        auto files = m_file_panel->model()->getMarkedFilesLocal();
        sol::table table = lua.create_table();
        if (files.isEmpty())
            return table;
        for (int i=0; i < files.size(); i++)
            table[i + 1] = files.at(i);
        return table;
    }
    inline int local_marks_count() noexcept {
        return m_file_panel->model()->getMarkedFilesCountLocal();
    }
    inline int global_marks_count() noexcept {
        return m_file_panel->model()->getMarkedFilesCount();
    }
    inline bool has_selection() noexcept {
        return m_file_panel->has_selection();
    }
    inline sol::table get_inputbar_props(sol::state &lua) noexcept {
        sol::table table = lua.create_table();
        table["font"] = m_inputbar->font().family().toStdString();
        table["foreground"] = m_inputbar->Get_foreground_color();
        table["background"] = m_inputbar->Get_background_color();
        table["font_size"] = m_inputbar->get_font_size();
        return table;
    }
    inline int get_inputbar_font_size() noexcept {
        return m_inputbar->font().pixelSize();
    }
    inline void set_inputbar_font_size(const int &pixel_size) noexcept {
        m_inputbar->set_font_size(pixel_size);
    }
    inline std::string Get_terminal() noexcept { return m_terminal.toStdString(); }

    void Set_auto_save_bookmarks(const bool &state) noexcept;
    void Set_toolbar_icons_only() noexcept;
    void Set_toolbar_text_only() noexcept;
    void set_toolbar_layout(const sol::table &layout) noexcept;
    void Error(const QString &reason) noexcept;
    void update_lua_package_path(sol::state &lua) noexcept;

    inline FilePanel::ItemProperty item_property() noexcept {
        return m_file_panel->getItemProperty();
    }

    inline int count_item() noexcept {
        return m_file_panel->ItemCount();
    }

    sol::table list_runtime_paths(sol::this_state L) noexcept;

    void update_runtime_paths(const std::string &rtps) noexcept;

    inline std::string current_directory() noexcept {
        return m_file_panel->getCurrentDir().toStdString();
    }

    inline std::string current_item_name() noexcept {
        return m_file_panel->getCurrentItem().toStdString();
    }

    inline Statusbar* statusbar() noexcept { return m_statusbar; }


    inline Statusbar::Module create_statusbar_module(const std::string &name,
                                                     const sol::table &options) noexcept {
        return m_statusbar->Lua__CreateModule(name, options);
    }

    inline void set_statusbar_modules(const sol::table &table) noexcept {
        m_statusbar->Lua__SetModules(table);
    }

    inline void add_statusbar_module(const Statusbar::Module &module) noexcept {
        m_statusbar->Lua__AddModule(module);
    }

    inline void set_statusbar_module_text(const std::string &name,
                                          const std::string &value) noexcept {
        m_statusbar->Lua__UpdateModuleText(name, value);
    }

    inline void toggle_toolbar() noexcept {
        m_toolbar->setVisible(!m_toolbar->isVisible());
    }


    inline void create_toolbar_button(const std::string &name, const sol::table &opts) noexcept {
        Lua__CreateToolbarButton(name, opts);
    }

    inline void set_statusbar_visible(const bool &state) noexcept {
        ToggleStatusBar(state);
    }

    inline bool get_statusbar_visible() noexcept {
        return m_statusbar->isVisible();
    }

    inline void set_menubar_visible(const bool &state) noexcept {
        ToggleMenuBar(state);
    }

    inline bool get_menubar_visible() noexcept {
        return m_menubar->isVisible();
    }

    void set_toolbar_props(const sol::table &table) noexcept;

    inline void set_toolbar_visible(const bool &state) noexcept {
        m_toolbar->setVisible(state);
    }

    inline bool get_toolbar_visible() noexcept {
        return m_toolbar->isVisible();
    }

    inline void set_pathbar_visible(const bool &state) noexcept {
        TogglePathWidget(state);
    }

    inline bool get_pathbar_visible() noexcept {
        return m_file_path_widget->isVisible();
    }

    inline void set_preview_panel_visible(const bool &state) noexcept {
        TogglePreviewPanel(state);
    }

    inline bool get_preview_panel_visible() noexcept {
        return m_preview_panel->isVisible();
    }

    inline bool get_inputbar_visible() noexcept {
        return m_inputbar->isVisible();
    }

    inline void set_bookmark_auto_save(const bool &state) noexcept {
        m_auto_save_bookmarks = state;
    }

    inline bool get_bookmark_auto_save() noexcept {
        return m_auto_save_bookmarks;
    }


    inline void set_bulk_rename_editor(const std::string &editor) noexcept {
        m_file_panel->SetBulkRenameEditor(QString::fromStdString(editor));
    }

    inline void set_bulk_rename_with_terminal(const bool &term) noexcept {
        m_file_panel->SetBulkRenameWithTerminal(term);
    }

    inline void set_bulk_rename_file_threshold(const int &n) noexcept {
        m_file_panel->SetBulkRenameThreshold(n);
    }

    inline std::string get_bulk_rename_editor() noexcept {
        return m_file_panel->Bulk_rename_editor();
    }

    inline bool get_bulk_rename_with_terminal() noexcept {
        return m_file_panel->Is_bulk_rename_with_terminal();
    }

    inline int get_bulk_rename_file_threshold() noexcept {
        return m_file_panel->Bulk_rename_threshold();
    }

    inline void set_toolbar_icons_only(const bool &state) noexcept {
        if (state)
            Set_toolbar_icons_only();
        else
            Set_toolbar_text_only();
    }

    inline bool get_toolbar_icons_only() noexcept {
        return m_toolbar_icons_only;
    }

    inline sol::table get_toolbar_layout(sol::this_state L) noexcept {
        sol::state_view lua(L);
        sol::table table = lua.create_table();

        for (int i=0; i < m_toolbar_layout.size(); i++)
            table[i + 1] = m_toolbar_layout.at(i);

        return table;
    }


    inline void set_preview_Panel_max_file_size(const std::string &max_file_size) noexcept {
        m_preview_panel->set_max_file_size_threshold(utils::parseFileSize(QString::fromStdString(max_file_size)));
    }

    inline std::string get_preview_Panel_max_file_size() const noexcept {
        return m_preview_panel->max_preview_threshold();
    }

    inline void set_preview_panel_image_dimension(const sol::table &table) noexcept {
        if (table["width"].valid() && table["height"].valid()) {
            auto width = table["width"].get<int>();
            auto height = table["height"].get<int>();
            m_preview_panel->Set_preview_dimension(width, height);
        }
    }

    inline sol::table get_preview_panel_image_dimension(sol::this_state L) const noexcept {
        sol::state_view lua(L);
        auto [width, height] = m_preview_panel->get_preview_dimension();
        sol::table table = lua.create_table();
        table["width"] = width;
        table["height"] = height;
        return table;
    }

    inline bool get_menubar_icons() const noexcept {
        return m_menubar_icons;
    }

    inline void set_cycle(const bool &state) noexcept {
        m_file_panel->SetCycle(state);
    }

    inline bool get_cycle() const noexcept {
        return m_file_panel->get_cycle();
    }

    inline void set_header_visible(const bool &state) noexcept {
        m_file_panel->ToggleHeaders(state);
    }

    inline bool get_header_visible() const noexcept {
        return m_file_panel->get_headers_visible();
    }

    inline sol::table get_header_columns(sol::this_state L) const noexcept {
        sol::state_view lua(L);
        sol::table table;
        auto cols = m_file_panel->model()->get_columns();
        for (int i=0; i < cols.size(); i++)
            table[i + 1] = cols.at(i);
        return table;
    }

    inline void set_preview_panel_props(const sol::table &table) noexcept {

        if (table["fraction"].valid())
            Set_preview_pane_fraction(table["fraction"].get<float>());

        if (table["max_file_size"].valid())
            set_preview_Panel_max_file_size(table["max_file_size"].get<std::string>());

        if (table["visible"].valid())
            set_preview_panel_visible(table["visible"].get<bool>());

        if (table["image_dimension"].valid())
            set_preview_panel_image_dimension(table["image_dimension"].get<sol::table>());

        if (table["read_num_lines"].valid())
            set_preview_panel_read_num_lines(table["image_dimension"].get<int>());

        if (table["fraction"].valid())
            Set_preview_pane_fraction(table["fraction"].get<float>());

    }

    inline void set_preview_panel_read_num_lines(const int &num) noexcept {
        m_preview_panel->set_num_read_lines(num);
    }

    inline int get_preview_panel_read_num_lines() const noexcept {
        return m_preview_panel->get_num_read_lines();
    }

    sol::table get_preview_panel_props(sol::this_state L) const noexcept;

    void set_header_columns(const sol::table &table) noexcept;

    inline void set_visual_line_mode_text(const std::string &text) noexcept {
        m_statusbar->SetVisualLineModeText(QString::fromStdString(text));
    }

    inline void set_visual_line_mode_italic(const bool &state) noexcept {
        m_statusbar->SetVisualLineModeItalic(state);
    }

    inline void set_visual_line_mode_bold(const bool &state) noexcept {
        m_statusbar->SetVisualLineModeBold(state);
    }

    inline void set_visual_line_mode_padding(const std::string &padding) noexcept {
        m_statusbar->SetVisualLineModePadding(QString::fromStdString(padding));
    }

    inline std::string get_visual_line_mode_padding() const noexcept {
        return m_statusbar->get_visual_line_mode_padding().toStdString();
    }

    inline bool get_visual_line_mode_italic() const noexcept {
        return m_statusbar->get_visual_line_mode_italic();
    }

    inline bool get_visual_line_mode_bold() const noexcept {
        return m_statusbar->get_visual_line_mode_bold();
    }

    inline void set_visual_line_mode_background(const std::string &bg) noexcept {
        m_statusbar->SetVisualLineModeBackground(QString::fromStdString(bg));
    }

    inline void set_visual_line_mode_foreground(const std::string &fg) noexcept {
        m_statusbar->SetVisualLineModeForeground(QString::fromStdString(fg));
    }

    inline std::string get_visual_line_mode_foreground() const noexcept {
        return m_statusbar->get_visual_line_mode_foreground().toStdString();
    }

    inline std::string get_visual_line_mode_background() const noexcept {
        return m_statusbar->get_visual_line_mode_background().toStdString();
    }

    inline void set_file_panel_icons(const bool &state) noexcept {
        m_file_panel->set_icons(state);
    }


    inline bool get_file_panel_icons() const noexcept {
        return m_file_panel->icons_enabled();
    }

    inline void set_file_panel_font_size(const int &size) noexcept {
        m_file_panel->set_font_size(size);
    }

    inline int get_file_panel_font_size() const noexcept {
        return m_file_panel->get_font_size();
    }

    inline void set_file_panel_font(const std::string &font) noexcept {
        m_file_panel->set_font_family(QString::fromStdString(font));
    }

    inline std::string get_file_panel_font() const noexcept {
        return m_file_panel->get_font_family().toStdString();
    }

    inline void set_file_panel_props(const sol::table &table) noexcept {

        if (table["font"])
            m_file_panel->set_font_family(QString::fromStdString(table["font"].get<std::string>()));

        if (table["font_size"])
            m_file_panel->set_font_size(table["font_size"].get<int>());

        if (table["icons"])
            m_file_panel->set_icons(table["icons"].get<bool>());
    }

    inline sol::table get_file_panel_props() noexcept {

        sol::table table = m_lua->create_table();
        table["font"] = m_file_panel->get_font_family().toStdString();
        table["font_size"] = m_file_panel->get_font_size();
        table["icons"] = m_file_panel->icons_enabled();

        return table;
    }

    // This is used to set all of navi api to a stringlist which is
    // then used to lua completions
    void set_api_list(const QStringList &list) noexcept;

    inline void set_pathbar_bold(const bool &state) noexcept {
        m_file_path_widget->setBold(state);
    }

    inline bool get_pathbar_bold() const noexcept {
        return m_file_path_widget->bold();
    }

    inline void set_pathbar_italic(const bool &state) noexcept {
        m_file_path_widget->setItalic(state);
    }

    inline bool get_pathbar_italic() const noexcept {
        return m_file_path_widget->italic();
    }

    void set_pathbar_props(const sol::table &table) noexcept;

protected:
    bool event(QEvent *e) override;

private:
    void init_default_options() noexcept;
    void initToolbar() noexcept;
    void onQuit() noexcept;
    void initConfiguration() noexcept;
    void chmodHelper() noexcept;
    void initBookmarks() noexcept;
    void initLayout() noexcept;
    void initMenubar() noexcept;
    void initSignalsSlots() noexcept;
    void setupCommandMap() noexcept;
    bool isValidPath(QString path);
    void initKeybinds() noexcept;
    bool createEmptyFile(const QString &filename) noexcept;
    QString getCurrentFile() noexcept;
    void ProcessCommand(const QString &commandtext) noexcept;
    void generateKeybinds() noexcept;
    void initTabBar() noexcept;
    void addCommandToMacroRegister(const QStringList &commandlist) noexcept;
    void addCommandToMacroRegister(const QString &command) noexcept;
    void cacheThumbnails() noexcept;

    QWidget *m_widget = new QWidget();
    QVBoxLayout *m_layout = new QVBoxLayout();
    QSplitter *m_splitter = new QSplitter();
    Menubar *m_menubar = nullptr;

    // Menubar stuff
    QMenu *m_filemenu = nullptr;
    QMenu *m_viewmenu = nullptr;
    QMenu *m_tools_menu = nullptr;
    QMenu *m_edit_menu = nullptr;
    QMenu *m_bookmarks_menu = nullptr;
    QMenu *m_go_menu = nullptr;
    QMenu *m_help_menu = nullptr;

    QMenu *m_bookmarks_menu__bookmarks_list_menu = nullptr;

    QMenu *m_filemenu__create_new_menu = nullptr;
    QAction *m_filemenu__new_window = nullptr;
    QAction *m_filemenu__create_new_folder = nullptr;
    QAction *m_filemenu__create_new_file = nullptr;
    QAction *m_filemenu__close_window = nullptr;
    QAction *m_filemenu__folder_properties = nullptr;


    QAction *m_viewmenu__refresh = nullptr;
    QAction *m_viewmenu__filter = nullptr;
    QAction *m_viewmenu__preview_panel = nullptr;
    QAction *m_viewmenu__menubar = nullptr;
    QAction *m_viewmenu__fullscreen = nullptr;
    QAction *m_viewmenu__statusbar = nullptr;
    QAction *m_viewmenu__headers = nullptr;
    QAction *m_viewmenu__messages = nullptr;
    QAction *m_viewmenu__marks_buffer = nullptr;
    QAction *m_viewmenu__bookmarks_buffer = nullptr;
    QAction *m_viewmenu__shortcuts_widget = nullptr;
    QAction *m_viewmenu__drives_widget = nullptr;
    QAction *m_viewmenu__tasks_widget = nullptr;

    QMenu *m_viewmenu__sort_menu = nullptr;

    QActionGroup *m_viewmenu__sort_by_group = nullptr;
    QAction *m_viewmenu__sort_by_name = nullptr;
    QAction *m_viewmenu__sort_by_size = nullptr;
    QAction *m_viewmenu__sort_by_date = nullptr;

    QActionGroup *m_viewmenu__sort_asc_desc_group = nullptr;
    QAction *m_viewmenu__sort_ascending = nullptr;
    QAction *m_viewmenu__sort_descending = nullptr;

    QMenu *m_viewmenu__files_menu = nullptr;
    QAction *m_viewmenu__files_menu__dotdot = nullptr;
    QAction *m_viewmenu__files_menu__hidden = nullptr;

    QAction *m_bookmarks_menu__add = nullptr;
    QAction *m_bookmarks_menu__remove = nullptr;

    QAction *m_edit_menu__open = nullptr;
    QAction *m_edit_menu__copy = nullptr;
    QAction *m_edit_menu__paste = nullptr;
    QAction *m_edit_menu__cut = nullptr;
    QAction *m_edit_menu__rename = nullptr;
    QAction *m_edit_menu__delete = nullptr;
    QAction *m_edit_menu__trash = nullptr;
    QAction *m_edit_menu__copy_path = nullptr;
    QAction *m_edit_menu__item_property = nullptr;
    QAction *m_edit_menu__select_all = nullptr;
    QAction *m_edit_menu__select_inverse = nullptr;

    QAction *m_tools_menu__search = nullptr;
    QAction *m_tools_menu__find_files = nullptr;

    QAction *m_go_menu__previous_folder = nullptr;
    QAction *m_go_menu__next_folder = nullptr;
    QAction *m_go_menu__parent_folder = nullptr;
    QAction *m_go_menu__home_folder = nullptr;
    QAction *m_go_menu__connect_to_server = nullptr;

    QAction *m_help_menu__about = nullptr;

    Inputbar *m_inputbar = nullptr;
    FilePanel *m_file_panel = nullptr;
    PreviewPanel *m_preview_panel = nullptr;
    FilePathWidget *m_file_path_widget = nullptr;
    Statusbar *m_statusbar = nullptr;

    // Hashmap for storing the commands and the corresponding function calls
    QHash<QString, std::function<void(const QStringList &args)>> commandMap;

    QStringList m_valid_command_list = {
        // Shell command
        "shell",

        // Mark
        "mark",
        "toggle-mark",
        "toggle-mark-dwim",
        "mark-inverse",
        "mark-all",
        "mark-dwim",
        "mark-regex",

        // Unmark
        "unmark",
        "unmark-global",
        "unmark-local",
        "unmark-dwim",
        "unmark-regex",

        // Chmod
        "chmod",
        "chmod-global",
        "chmod-local",
        "chmod-dwim",

        // Rename
        "rename",
        "rename-global",
        "rename-local",
        "rename-dwim",

        // Cut
        "cut",
        "cut-local",
        "cut-global",
        "cut-dwim",

        // Copy
        "copy",
        "copy-local",
        "copy-global",
        "copy-dwim",

        // Paste
        "paste",

        // Delete
        "delete",
        "delete-local",
        "delete-global",
        "delete-dwim",

        // Trash
        "trash",
        "trash-local",
        "trash-global",
        "trash-dwim",

        // Task
        "tasks",

        // Bookmarks
        "bookmark-add",
        "bookmark-remove",
        "bookmark-edit-name",
        "bookmark-edit-path",
        "bookmark-go",
        "bookmarks-save",

        // Creation
        "new-file",
        "new-folder",

        // Panes
        "messages-pane",
        "preview-pane",
        "marks-pane",
        "bookmarks-pane",
        "shortcuts-pane",

        // Search
        "search",
        "search-regex",
        "search-next",
        "search-prev",

        // Sort
        "sort-name",
        "sort-name-desc",
        "sort-date",
        "sort-date-desc",
        "sort-size",
        "sort-size-desc",

        // Navigation
        "next-item",
        "prev-item",
        "first-item",
        "last-item",
        "middle-item",
        "up-directory",
        "select-item",

        "macro-play",
        "macro-record",
        "macro-delete",
        "macro-list",
        "macro-edit",
        "macro-save-to-file",

        // Echo
        "echo-info",
        "echo-warn",
        "echo-error",

        // misc
        "new-window",
        "exit",
        "filter",
        "reset-filter",
        "refresh",
        "hidden-files",
        "dot-dot",
        "menu-bar",
        "focus-path",
        "item-property",
        "cycle",
        "header",
        "reload-config",
        "execute-extended-command",
        "visual-select",
        "mouse-scroll",
        "drives",
        // "syntax-highlight",
        "lua",
        "register",
        "repeat-last-command",
        "cd",
        "terminal",
        "folder-property",
        "copy-path",
        "fullscreen",
        "about",
    };

    MessagesBuffer *m_log_buffer = nullptr;
    MarksBuffer *m_marks_buffer = nullptr;

    BookmarkManager *m_bookmark_manager = nullptr;
    BookmarkWidget *m_bookmarks_buffer = nullptr;

    QDir::SortFlags m_sort_flags = QDir::SortFlag::DirsFirst;
    enum class SortBy { Name = 0, Date, Size };
    SortBy m_sort_by = SortBy::Name;
    sol::state m_bookmarks_state;
    ShortcutsWidget *m_shortcuts_widget = nullptr;
    QList<Keybind> m_keybind_list;
    QString m_config_location = CONFIG_FILE_PATH;
    bool m_load_config = true;
    QString m_default_dir = "~";
    bool m_set_default_working_dir = true;
    DriveWidget *m_drives_widget = new DriveWidget(this);
    QStringList m_search_history_list = {};
    TaskManager *m_task_manager = new TaskManager(this);
    TasksWidget *m_tasks_widget = nullptr;
    RegisterWidget *m_register_widget = new RegisterWidget(this);
    QString m_terminal = "kitty";
    QStringList m_macro_register = {};
    QHash<QString, QStringList> m_macro_hash;
    bool m_auto_save_bookmarks = true;
    bool m_macro_mode = false;
    HookManager *m_hook_manager = nullptr;
    QClipboard *m_clipboard = QGuiApplication::clipboard();
    QString m_copy_path_join_str = "\n";
    QFuture<void> m_thumbnail_cache_future;
    QFutureWatcher<void> *m_thumbnail_cache_future_watcher = new QFutureWatcher<void>(this);
    QToolBar *m_toolbar = nullptr;
    QPushButton *m_toolbar__prev_btn = nullptr;
    QPushButton *m_toolbar__next_btn = nullptr;
    QPushButton *m_toolbar__home_btn = nullptr;
    QPushButton *m_toolbar__parent_btn = nullptr;
    QPushButton *m_toolbar__refresh_btn = nullptr;
    QSet<QString> m_runtime_path = {};
    double m_preview_pane_fraction = 0.5f;
    bool m_menubar_icons = true, m_toolbar_icons_only = true;
    std::vector<std::string> m_toolbar_layout;
    sol::state *m_lua = nullptr;
    QStringList m_navi_lua_api_list;
};
