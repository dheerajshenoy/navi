#pragma once

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QHash>
#include <QInputDialog>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndexList>
#include <QObject>
#include <QRegularExpression>
#include <QSet>
#include <QShortcut>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QProcess>
#include <QTemporaryFile>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QApplication>
#include <qnamespace.h>

#include "Result.hpp"
#include "FileSystemModel.hpp"
#include "FileWorker.hpp"
#include "TableView.hpp"
#include "Inputbar.hpp"
#include "FilePropertyWidget.hpp"
#include "Statusbar.hpp"


class FilePanel : public QWidget {
    Q_OBJECT
public:
    FilePanel(Inputbar *inputWidget = nullptr, Statusbar *statusBar = nullptr, QWidget *parent = nullptr);
    ~FilePanel();

    QTableView* tableView() { return m_table_view; }
    void setCurrentDir(QString path, const bool &selectFirstItem = false) noexcept;
    QString getCurrentDir() noexcept;
    QSet<QString> *getMarksSetPTR() noexcept { return &m_model->m_markedFiles; }
    FileSystemModel* model() noexcept { return m_model; }

    void highlightIndex(const QModelIndex &index) noexcept;
    void NewFolder(const QStringList &folderName = {}) noexcept;
    void NewFile(const QStringList &fileName = {}) noexcept;
    QString getCurrentItem() noexcept;
    QString getCurrentItemFileName() noexcept;
    void UpDirectory() noexcept;
    void SelectItem() noexcept;
    void SelectItemHavingString(const QString &item) noexcept;
    void NextItem() noexcept;
    void PrevItem() noexcept;

    void AsyncShellCommand(const QString &command) noexcept;
    void ShellCommand(const QString &command) noexcept;

    void ToggleMarkItem() noexcept;
    void ToggleMarkDWIM() noexcept;
    void MarkItem() noexcept;
    void MarkItems(const QModelIndexList &indexes) noexcept;
    void MarkInverse() noexcept;
    void MarkAllItems() noexcept;
    void MarkDWIM() noexcept;

    void UnmarkItem() noexcept;
    void UnmarkItems(const QModelIndexList &list) noexcept;
    void UnmarkItemsLocal() noexcept;
    void UnmarkItemsGlobal() noexcept;
    void UnmarkDWIM() noexcept;

    void GotoFirstItem() noexcept;
    void GotoLastItem() noexcept;
    void GotoItem(const uint &itemNum) noexcept;
    void GotoMiddleItem() noexcept;

    void RenameItem() noexcept;
    void RenameItems(const QStringList &files) noexcept;
    void RenameItemsGlobal() noexcept;
    void RenameItemsLocal() noexcept;
    void RenameDWIM() noexcept;

    void PasteItems() noexcept;

    void DeleteItem() noexcept;
    void DeleteItems(const QStringList &files) noexcept;
    void DeleteItemsLocal() noexcept;
    void DeleteItemsGlobal() noexcept;
    void DeleteDWIM() noexcept;

    void TrashItem() noexcept;
    void TrashItems(const QStringList &files) noexcept;
    void TrashItemsLocal() noexcept;
    void TrashItemsGlobal() noexcept;
    void TrashDWIM() noexcept;

    void ToggleDotDot() noexcept;
    void ToggleHiddenFiles() noexcept;
    void Search(QString searchText = "") noexcept;
    void SearchNext() noexcept;
    void SearchPrev() noexcept;
    void Filters(const QString &filterString) noexcept;
    void ResetFilter() noexcept;
    void ForceUpdate() noexcept;
    void ChmodItem() noexcept;
    void ChmodItemsLocal() noexcept;
    void ChmodItemsGlobal() noexcept;
    bool SetPermissions(const QString &filePath,
                        const QString &permString) noexcept;
    void CopyItem() noexcept;
    void CopyItemsLocal() noexcept;
    void CopyItemsGlobal() noexcept;
    void CopyDWIM() noexcept;

    void CutItem() noexcept;
    void CutItemsLocal() noexcept;
    void CutItemsGlobal() noexcept;
    void CutDWIM() noexcept;

    void DropCopyRequested(const QStringList &sourcePaths) noexcept;
    void DropCutRequested(const QStringList &sourcePaths) noexcept;
    void ItemProperty() noexcept;
    Result<bool> OpenTerminal(const QString &directory = "") noexcept;
    void BulkRename(const QStringList &files) noexcept;
    void ToggleHeaders(const bool &state) noexcept;
    void ToggleHeaders() noexcept;
    void SetCycle(const bool &state) noexcept;
    void ToggleCycle() noexcept;
    void OpenWith() noexcept;

    void ToggleVisualLine() noexcept;
    void ToggleVisualLine(const bool &state) noexcept;

    void ToggleMouseScroll() noexcept;
    void ToggleMouseScroll(const bool &state) noexcept;

    inline void setCurrentForeground(const QString &color) noexcept {
        m_current_foreground = color;

        m_table_view->setStyleSheet(QString("QTableView::item:selected"
                                            "{"
                                            "background-color : %1;"
                                            "selection-color : %2;"
                                            "}")
                                  .arg(m_current_background.name())
                                  .arg(m_current_foreground.name()));
    }

    inline void setCurrentBackground(const QString &color) noexcept {
        m_current_background = color;

        m_table_view->setStyleSheet(QString("QTableView::item:selected"
                                            "{"
                                            "background-color : %1;"
                                            "selection-color : %2;"
                                            "}")
                                  .arg(m_current_background.name())
                                  .arg(m_current_foreground.name()));
    }

signals:
    void afterDirChange(QString path);
    void currentItemChanged(const QString &path);
    void fileOperationDone(const bool &result, const QString &reason = 0);
    void dropCopyRequested(const QStringList &sourcePath);
    void dropCutRequested(const QStringList &sourcePath);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions);
    void wheelEvent(QWheelEvent *event) override;
    // void mousePressEvent(QMouseEvent *event) override;
    // void mouseMoveEvent(QMouseEvent *event) override;

private:
    void dragRequested() noexcept;
    void selectHelper(const QModelIndex &index, const bool selectFirst) noexcept;
    void selectFirstItem() noexcept;
    void initContextMenu() noexcept;
    void handleItemDoubleClicked(const QModelIndex &index) noexcept;
    void initSignalsSlots() noexcept;
    QVBoxLayout *m_layout = new QVBoxLayout();
    TableView *m_table_view = new TableView();
    FileSystemModel *m_model = new FileSystemModel();
    QString m_current_dir;
    bool m_hidden_files_shown = false;
    bool m_hidden_files_just_toggled = false;
    bool m_dot_dot_shown = false;
    QAction *m_context_action_open = nullptr;
    QAction *m_context_action_open_with = nullptr;
    QAction *m_context_action_cut = nullptr;
    QAction *m_context_action_copy = nullptr;
    QAction *m_context_action_paste = nullptr;
    QAction *m_context_action_delete = nullptr;
    QAction *m_context_action_trash = nullptr;
    QAction *m_context_action_rename = nullptr;
    QAction *m_context_action_properties = nullptr;
    QAction *m_context_action_open_terminal = nullptr;
    // Search related things
    QModelIndexList m_search_index_list;
    int m_search_index_list_index = -1;
    unsigned int m_item_count = 0;
    // This variable is used to check if the search has been performed
    // in the directory. If not, then we perform check and then
    // iterate through the search results. If yes, we just iterate
    bool m_search_new_directory = true;
    QString m_search_text;
    // This is for storing the recent file operation action like COPY, PASTE.
    // Depending on which we perform the necessary action.
    FileOPType m_file_op_type = FileOPType::COPY;
    QString m_terminal = getenv("TERMINAL");
    QStringList m_terminal_args;
    Inputbar *m_inputbar;
    Statusbar *m_statusbar;
    // This is used to store the filenames that are marked for cutting or copying.
    QStringList m_register_files_list;
    QPoint m_drag_start_position;
    bool m_cycle_item = true;
    bool m_visual_line_mode = false;
    bool m_scroll_action = true;
    unsigned int m_bulk_rename_threshold = 5;
    QModelIndex m_visual_start_index;
    int m_file_name_column_index = -1;
    QColor m_current_background, m_current_foreground;
    int m_highlight_row;
    QString m_highlight_text;
    QHash <QString, QString> m_default_applications_hash;
};