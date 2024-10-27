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

// Local includes
#include "FilePathWidget.hpp"
#include "Inputbar.hpp"
#include "MessagesBuffer.hpp"
#include "FilePanel.hpp"
#include "PreviewPanel.hpp"
#include "Statusbar.hpp"
#include "MarksBuffer.hpp"

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

    void setCurrentDir(const QString &path) noexcept;

    // Interactive Functions
    void TogglePreviewPanel(const bool &state) noexcept;
    void TogglePreviewPanel() noexcept;
    void ExecuteExtendedCommand() noexcept;
    void RenameItems() noexcept;
    void NewFile(const int &nfiles = -1) noexcept;
    void NewFolder(const int &nfolders = -1) noexcept;
    void CutItems() noexcept;
    void CopyItems() noexcept;
    void DeleteItems() noexcept;
    void TrashItems() noexcept;
    void ShowHelp() noexcept;
    void Search() noexcept;
    void SearchNext() noexcept;
    void SearchPrev() noexcept;
    void ToggleMenuBar(const bool &state) noexcept;
    void ToggleMenuBar() noexcept;
    void Filter() noexcept;
    void ResetFilter() noexcept;
    void Chmod() noexcept;
    void HighlightMode() noexcept;
    void PasteItems() noexcept;
    void LogMessage(const QString &message, const MessageType &type) noexcept;
    void ToggleMessagesBuffer(const bool &state) noexcept;
    void ToggleMessagesBuffer() noexcept;
    void ToggleMarksBuffer(const bool &state) noexcept;
    void ToggleMarksBuffer() noexcept;
    void UnmarkAllItems() noexcept;

private:
    void initLayout() noexcept;
    void initMenubar() noexcept;
    void initSignalsSlots() noexcept;
    void setupCommandMap() noexcept;
    bool isValidPath(QString path);
    void initKeybinds() noexcept;
    bool createEmptyFile(const QString &filename) noexcept;
    QString getCurrentFile() noexcept;
    void ProcessCommand(const QString &commandtext) noexcept;


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
    QAction *m_viewmenu__messages = nullptr;
    QAction *m_viewmenu__marks_buffer = nullptr;

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

    FilePanel *m_file_panel = nullptr;
    PreviewPanel *m_preview_panel = nullptr;
    FilePathWidget *m_file_path_widget = nullptr;
    Statusbar *m_statusbar = nullptr;
    Inputbar *m_inputbar = nullptr;

    // Hashmap for storing the commands and the corresponding function calls
    QHash<QString, std::function<void(void)>> commandMap;

    QStringList m_valid_command_list = {
        "mark",
        "unmark",
        "unmark-all",
        "chmod",
        "toggle-mark",
        "rename",
        "cut",
        "copy",
        "paste",
        "cut",
        "delete",
        "trash",
        "messages",
        "filter",
        "unfilter",
        "refresh",
        "toggle-hidden-files",
        "toggle-preview-pane",
        "toggle-menu-bar",
    };

    QStringListModel *m_input_completion_model = nullptr;

    MessagesBuffer *m_log_buffer = nullptr;
    MarksBuffer *m_marks_buffer = nullptr;
};