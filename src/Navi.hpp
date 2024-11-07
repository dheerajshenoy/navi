#pragma once

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
#include "sol/sol.hpp"
#include "argparse.hpp"

class ShortcutWidget;

// Config related things
static const QString APP_NAME = "navi";
static const QString CONFIG_DIR_PATH =
    QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
    QDir::separator() + APP_NAME;
static const QString CONFIG_FILE_NAME = "config.lua";
static const QString CONFIG_FILE_PATH =
    CONFIG_DIR_PATH + QDir::separator() + CONFIG_FILE_NAME;

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
    void setCurrentDir(const QString &path) noexcept;
    // Interactive Functions
    void ExecuteExtendedCommand() noexcept;
    void NewFolder(const QStringList &folders = {}) noexcept;
    void PasteItems() noexcept;
    void ShowHelp() noexcept;
    void ToggleMenuBar(const bool &state) noexcept;
    void ToggleMenuBar() noexcept;
    void Filter() noexcept;
    void ResetFilter() noexcept;
    void LogMessage(const QString &message, const MessageType &type) noexcept;
    void FocusPath() noexcept;
    void AddBookmark(const QStringList &bookmarkName) noexcept;
    void RemoveBookmark(const QStringList &bookmarkName) noexcept;
    void EditBookmark(const QStringList &bookmarkName) noexcept;
    void LoadBookmarkFile(const QStringList &bookmarkFilePath) noexcept;
    void GoBookmark(const QStringList &bookmarkName) noexcept;
    void SaveBookmarkFile() noexcept;
    void SortByName(const bool &reverse = false) noexcept;
    void SortByDate(const bool &reverse = false) noexcept;
    void SortBySize(const bool &reverse = false) noexcept;

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

    void readArgumentParser(argparse::ArgumentParser &parser);

private:
    void initConfiguration();
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


    QWidget *m_widget = new QWidget();
    QVBoxLayout *m_layout = new QVBoxLayout();
    QSplitter *m_splitter = new QSplitter();
    Menubar *m_menubar = nullptr;

    // Menubar stuff
    QMenu *m_filemenu = nullptr;
    QMenu *m_viewmenu = nullptr;
    QMenu *m_tools_menu = nullptr;
    QMenu *m_edit_menu = nullptr;

    QMenu *m_filemenu__create_new_menu = nullptr;
    QAction *m_filemenu__new_window = nullptr;
    QAction *m_filemenu__new_tab = nullptr;
    QAction *m_filemenu__create_new_folder = nullptr;
    QAction *m_filemenu__create_new_file = nullptr;

    QAction *m_viewmenu__preview_panel = nullptr;
    QAction *m_viewmenu__menubar = nullptr;
    QAction *m_viewmenu__statusbar = nullptr;
    QAction *m_viewmenu__headers = nullptr;
    QAction *m_viewmenu__messages = nullptr;
    QAction *m_viewmenu__marks_buffer = nullptr;
    QAction *m_viewmenu__bookmarks_buffer = nullptr;
    QAction *m_viewmenu__shortcuts_widget = nullptr;

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

    QAction *m_tools_menu__search = nullptr;
    QAction *m_tools_menu__command_in_folder = nullptr;

    QAction *m_edit_menu__copy = nullptr;
    QAction *m_edit_menu__paste = nullptr;
    QAction *m_edit_menu__cut = nullptr;
    QAction *m_edit_menu__rename = nullptr;
    QAction *m_edit_menu__delete = nullptr;
    QAction *m_edit_menu__trash = nullptr;

    Inputbar *m_inputbar = nullptr;
    FilePanel *m_file_panel = nullptr;
    PreviewPanel *m_preview_panel = nullptr;
    FilePathWidget *m_file_path_widget = nullptr;
    Statusbar *m_statusbar = nullptr;

    // Hashmap for storing the commands and the corresponding function calls
    QHash<QString, std::function<void(const QStringList &args)>> commandMap;

    QStringList m_valid_command_list = {

      // Shell command
      "shell-command",
      "shell-command-async",

      // Mark
      "mark",
      "toggle-mark",
      "toggle-mark-dwim",
      "mark-inverse",
      "mark-all",
      "mark-dwim",

      // Unmark
      "unmark",
      "unmark-global",
      "unmark-local",
      "unmark-dwim",

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

      // Bookmarks
      "bookmark-add",
      "bookmark-remove",
      "bookmark-edit",
      "bookmark-go",
      "bookmarks-save",

      // Panes
      "messages-pane",
      "preview-pane",
      "marks-pane",
      "bookmarks-pane",
      "shortcuts-pane",

      // Search
      "search",
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

      // Echo
      "echo-info",
      "echo-warn",
      "echo-error",

      // misc
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

    };

    QStringListModel *m_input_completion_model = nullptr;

    MessagesBuffer *m_log_buffer = nullptr;
    MarksBuffer *m_marks_buffer = nullptr;

    BookmarkManager *m_bookmark_manager = nullptr;
    BookmarkWidget *m_bookmarks_buffer = nullptr;

    QDir::SortFlags m_sort_flags = QDir::SortFlag::DirsFirst;
    enum class SortBy { Name = 0, Date, Size };
    SortBy m_sort_by = SortBy::Name;
    sol::state lua;

    ShortcutsWidget *m_shortcuts_widget = nullptr;

    QList<Keybind> m_keybind_list;

    QString m_config_location = CONFIG_FILE_PATH;
    bool m_load_config = true;

};