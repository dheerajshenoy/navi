#pragma once

#define UNUSED(x) (void) x

// #define SOL_ALL_SAFETIES_ON 1

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

// Config related things
static const QString APP_NAME = "navi";
static const QString CONFIG_DIR_PATH =
    QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
    QDir::separator() + APP_NAME;
static const QString CONFIG_FILE_NAME = "config.lua";
static const QString CONFIG_FILE_PATH =
    CONFIG_DIR_PATH + QDir::separator() + CONFIG_FILE_NAME;
static const QString BOOKMARK_FILE_NAME = "bookmark.lua";
static const QString BOOKMARK_FILE_PATH = CONFIG_DIR_PATH + QDir::separator() + BOOKMARK_FILE_NAME;

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

    void initThings() noexcept;
    void SelectAllItems() noexcept;
    void SelectInverse() noexcept;
    // Interactive Functions
    void ExecuteExtendedCommand() noexcept;
    void NewFolder(const QStringList &folders = {}) noexcept;
    void NewFile(const QStringList &files = {}) noexcept;
    void PasteItems() noexcept;
    void ShowHelp() noexcept;
    void ToggleMenuBar(const bool &state) noexcept;
    void ToggleMenuBar() noexcept;
    void Filter() noexcept;
    void ResetFilter() noexcept;
    void LogMessage(const QString &message, const MessageType &type = MessageType::INFO) noexcept;
    void FocusPath() noexcept;
    void AddBookmark(const QStringList &bookmarkName = QStringList()) noexcept;
    void RemoveBookmark(const QStringList &bookmarkName = QStringList()) noexcept;
    void SortDescending(const bool &state) noexcept;
    void SortAscending(const bool &state) noexcept;

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

    void EditBookmarkName(const QStringList &bookmarkName) noexcept;
    void EditBookmarkFile(const QStringList &bookmarkName) noexcept;
    void LoadBookmarkFile(const QStringList &bookmarkFilePath) noexcept;
    void GoBookmark(const QString &bookmarkName = QString()) noexcept;
    void SaveBookmarkFile() noexcept;
    void SortByName(const bool &reverse = false) noexcept;
    void SortByDate(const bool &reverse = false) noexcept;
    void SortBySize(const bool &reverse = false) noexcept;
    void ShellCommandAsync(const QString &command = "") noexcept;
    void Search() noexcept;
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
    void ExecuteLuaFunction(const QStringList &args);
    void MountDrive(const QString &driveName) noexcept;
    void UnmountDrive(const QString &driveName) noexcept;
    void readArgumentParser(argparse::ArgumentParser &parser);
    void GotoItem(const int &num) noexcept;
    void ChangeDirectory(const QString &dir = QString()) noexcept;
    void LaunchNewInstance() noexcept;
    void ShowFolderProperty() noexcept;
    void Exit() noexcept;
    void CopyPath(const QString &separator = QString()) noexcept;

    std::string Lua__Input(const std::string &prompt,
                           const std::string &default_value,
                           const std::string &default_selection) noexcept;
    void Lua__Shell(const std::string &command) noexcept;
    void Lua__CreateFolders(const std::vector<std::string> &paths) noexcept;
    void Lua__AddMenu(const sol::table &menu) noexcept;
    void Lua__AddContextMenu(const sol::table &cmenu) noexcept;
    Navi::MenuItem Lua__parseMenuItem(const sol::table &table) noexcept;
    void FullScreen() noexcept;
    void FullScreen(const bool &state) noexcept;
    void ShowAbout() noexcept;

protected:
    bool event(QEvent *e) override;

private:
    void initDefaults() noexcept;
    void initToolbar() noexcept;
    void initNaviLuaAPI() noexcept;
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
    QStringList getLuaFunctionNames() noexcept;
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
    sol::state lua, m_bookmarks_state;
    ShortcutsWidget *m_shortcuts_widget = nullptr;
    QList<Keybind> m_keybind_list;
    QString m_config_location = CONFIG_FILE_PATH;
    bool m_load_config = true;
    QStringList m_default_location_list;
    DriveWidget *m_drives_widget = new DriveWidget(this);
    QStringList m_search_history_list = {};
    TaskManager *m_task_manager = new TaskManager(this);
    TasksWidget *m_tasks_widget = nullptr;
    RegisterWidget *m_register_widget = new RegisterWidget(this);
    QString m_terminal;
    QStringList m_macro_register = {};
    QHash<QString, QStringList> m_macro_hash;
    bool m_auto_save_bookmarks = false;
    bool m_macro_mode = false;
    HookManager *m_hook_manager = nullptr;
    QClipboard *m_clipboard = QGuiApplication::clipboard();
    QString m_copy_path_join_str = "\n";

    QFuture<void> m_thumbnail_cache_future;
    QFutureWatcher<void> *m_thumbnail_cache_future_watcher = new QFutureWatcher<void>(this);

    QToolBar *m_toolbar = new QToolBar(this);
    QPushButton *m_toolbar__prev_btn = nullptr;
    QPushButton *m_toolbar__next_btn = nullptr;
    QPushButton *m_toolbar__home_btn = nullptr;
    QPushButton *m_toolbar__parent_btn = nullptr;
    QPushButton *m_toolbar__refresh_btn = nullptr;
};