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

#include "Result.hpp"
#include "FileSystemModel.hpp"
#include "FileWorker.hpp"
#include "TableView.hpp"
#include "Inputbar.hpp"
#include "FilePropertyWidget.hpp"

class FilePanel : public QWidget {
    Q_OBJECT
public:
    FilePanel(Inputbar *inputWidget = nullptr, QWidget *parent = nullptr);
    ~FilePanel();

    void setCurrentDir(QString path, const bool &selectFirstItem = false) noexcept;
    QString getCurrentDir() noexcept;

    QSet<QString> *getMarksSetPTR() noexcept { return &m_model->m_markedFiles; }

    FileSystemModel* model() noexcept { return m_model; }

    bool NewFolder(const QString &folderName) noexcept;
    bool NewFile(const QString &fileName) noexcept;
    QString getCurrentItem() noexcept;
    QString getCurrentItemBaseName() noexcept;
    void UpDirectory() noexcept;
    void SelectItem() noexcept;
    void SelectItemHavingString(const QString &item) noexcept;
    void NextItem() noexcept;
    void PrevItem() noexcept;
    void MarkOrUnmarkItems() noexcept;
    void MarkItems() noexcept;
    void MarkInverse() noexcept;
    void MarkAllItems() noexcept;
    void UnmarkItem() noexcept;
    void UnmarkItemsLocal() noexcept;
    void UnmarkItemsGlobal() noexcept;
    void GotoFirstItem() noexcept;
    void GotoLastItem() noexcept;
    void GotoItem(const uint &itemNum) noexcept;
    Result<bool> RenameItem() noexcept;
    Result<bool> RenameItemsGlobal() noexcept;
    Result<bool> RenameItemsLocal() noexcept;
    void PasteItems() noexcept;
    Result<bool> DeleteItem() noexcept;
    Result<bool> DeleteItemsLocal() noexcept;
    Result<bool> DeleteItemsGlobal() noexcept;
    bool TrashItems() noexcept;
    void ToggleHiddenFiles() noexcept;
    void Search(const QString &searchExpression) noexcept;
    void SearchNext() noexcept;
    void SearchPrev() noexcept;
    void Filters(const QString &filterString) noexcept;
    void ResetFilter() noexcept;
    void ForceUpdate() noexcept;
    bool Chmod(const QString &permString) noexcept;
    bool SetPermissions(const QString &filePath,
                        const QString &permString) noexcept;
    void CopyItems() noexcept;
    void CutItems() noexcept;
    void DropCopyRequested(const QStringList &sourcePaths) noexcept;
    void DropCutRequested(const QStringList &sourcePaths) noexcept;
    void ItemProperty() noexcept;
    Result<bool> OpenTerminal(const QString &directory = "") noexcept;
    Result<bool> BulkRename(const QStringList &files) noexcept;

signals:
    void afterDirChange(QString path);
    void currentItemChanged(const QString &path);
    void dirItemCount(const int &numItems);
    void fileOperationDone(const bool &result, const QString &reason = 0);
    void dropCopyRequested(const QStringList &sourcePath);
    void dropCutRequested(const QStringList &sourcePath);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void startDrag(Qt::DropActions supportedActions);

private:
    void selectHelper(const QModelIndex &index, const bool selectFirst) noexcept;
    void selectFirstItem() noexcept;
    void initKeybinds() noexcept;
    void initContextMenu() noexcept;
    void handleItemDoubleClicked(const QModelIndex &index) noexcept;
    void initSignalsSlots() noexcept;

    QVBoxLayout *m_layout = new QVBoxLayout();
    TableView *m_table_view = new TableView();
    FileSystemModel *m_model = new FileSystemModel();

    QString m_current_dir;

    bool m_hidden_files_shown = false;

    QAction *m_context_action_open = nullptr;
    QAction *m_context_action_open_with = nullptr;
    QAction *m_context_action_cut = nullptr;
    QAction *m_context_action_copy = nullptr;
    QAction *m_context_action_paste = nullptr;
    QAction *m_context_action_delete = nullptr;
    QAction *m_context_action_trash = nullptr;
    QAction *m_context_action_properties = nullptr;
    QAction *m_context_action_open_terminal = nullptr;

    QModelIndexList m_search_index_list;
    int m_search_index_list_index = -1;
    unsigned int m_item_count = 0;

    // This is for storing the recent file operation action like COPY, PASTE.
    // Depending on which we perform the necessary action.
    FileOPType m_file_op_type;

    QString m_terminal = getenv("TERMINAL");
    QStringList m_terminal_args;

    Inputbar *m_inputbar;

    // This is used to store the string of the current item under focus
    // which will then be used to focus the item again after the directory
    // reloads
    // QString m_old_item_name;
};