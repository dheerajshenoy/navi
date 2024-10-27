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

#include "Result.hpp"
#include "FileSystemModel.hpp"
#include "FileWorker.hpp"
#include "TableView.hpp"

class FilePanel : public QWidget {
    Q_OBJECT
public:
    FilePanel(QWidget *parent = nullptr);
    ~FilePanel();

    void setCurrentDir(QString path) noexcept;
    QString getCurrentDir() noexcept;

    QString getCurrentItem() noexcept;
    void UpDirectory() noexcept;
    void SelectItem() noexcept;
    void NextItem() noexcept;
    void PrevItem() noexcept;
    void MarkOrUnmarkItems() noexcept;
    void MarkItems() noexcept;
    void UnmarkItems() noexcept;
    void GotoFirstItem() noexcept;
    void GotoLastItem() noexcept;
    void GotoItem(const uint &itemNum) noexcept;
    Result<bool> RenameItems() noexcept;
    void PasteItems() noexcept;
    bool DeleteItems() noexcept;
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
    void DropCopyRequested(const QString &sourcePath) noexcept;
    void DropCutRequested(const QString &sourcePath) noexcept;
    Result<bool> OpenTerminal(const QString &directory = "") noexcept;

signals:
    void afterDirChange(QString path);
    void currentItemChanged(const QString &path);
    void dirItemCount(const int &numItems);
    void copyPastaDone(const bool &result, const QString &reason = 0);
    void cutPastaDone(const bool &result, const QString &reason = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    void selectFirstItem() noexcept;
    QString currentItem() noexcept;
    void initKeybinds() noexcept;
    void initContextMenu() noexcept;
    void handleItemDoubleClicked(const QModelIndex &index) noexcept;
    void initSignalsSlots() noexcept;
    bool isValidPath(QString path) noexcept;

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

    // This is the register used for storing the files that have been marked
    // and prepared for operation COPY or PASTE.
    QSet<QString> m_register_for_files;

    // This is for storing the recent file operation action like COPY, PASTE.
    // Depending on which we perform the necessary action.
    FileOPType m_file_op_type;

    QString m_terminal = getenv("TERMINAL");
    QStringList m_terminal_args;
};