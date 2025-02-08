#pragma once

#include "FilePropertyWidget.hpp"
#include "FileSystemModel.hpp"
#include "FileWorker.hpp"
#include "HookManager.hpp"
#include "Inputbar.hpp"
#include "Mimeutils.hpp"
#include "Statusbar.hpp"
#include "TableView.hpp"
#include "pch/pch_filepanel.hpp"
#include "utils.hpp"
#include "QuestionDialog.hpp"

class FilePanel : public QWidget {
  Q_OBJECT
public:
  FilePanel(Inputbar *inputWidget, Statusbar *statusBar, HookManager *hm,
            TaskManager *tm, QWidget *parent = nullptr);
  ~FilePanel();

  struct ItemProperty {
    std::string name;
    std::string size;
    std::string mimeName;
  };

  TableView *tableView() { return m_table_view; }
  void setCurrentDir(QString path,
                     const bool &selectFirstItem = false) noexcept;

  QString getCurrentDir() noexcept;
  QSet<QString> *getMarksSetPTR() noexcept { return &m_model->m_markedFiles; }
  FileSystemModel *model() noexcept { return m_model; }

  void highlightIndex(const QModelIndex &index) noexcept;
  void HighlightItem(const QString &itemName) noexcept;
  void HighlightItemWithBaseName(const QString &baseName) noexcept;
  void NewFolder(const QStringList &folderName = {}) noexcept;
  void NewFile(const QStringList &fileName = {}) noexcept;
  QString getCurrentItem() noexcept;
  bool is_current_item_a_file() noexcept;
  QString getCurrentItemFileName() noexcept;
  void UpDirectory() noexcept;
  void HomeDirectory() noexcept;
  void PreviousDirectory() noexcept;
  void SelectItem() noexcept;
  void SelectItemHavingString(const QString &item) noexcept;
  void NextItem() noexcept;
  void PrevItem() noexcept;
  inline int ItemCount() noexcept { return m_item_count; }
  const ItemProperty getItemProperty() noexcept;

  void AsyncShellCommand(const QString &command) noexcept;
  void ShellCommand(const QString &command) noexcept;

  inline void set_icons(const bool &state) noexcept {
    model()->icons_enabled = state;
  }

  inline void set_font_family(const QString &font) noexcept {
    QFont _font = this->font();
    _font.setFamily(font);
    setFont(_font);
  }

  inline QString get_font_family() noexcept { return font().family(); }

  inline void set_font_size(const int &size) noexcept {
    m_table_view->set_font_size(size);
  }

  inline int get_font_size() noexcept { return m_table_view->get_font_size(); }

  inline void set_icons_enabled(const bool &state) noexcept {
    model()->icons_enabled = state;
  }

  void ToggleMarkItem() noexcept;
  void ToggleMarkDWIM() noexcept;
  void MarkItem() noexcept;
  void MarkItems(const QModelIndexList &indexes) noexcept;
  void MarkInverse() noexcept;
  void MarkAllItems() noexcept;
  void MarkDWIM() noexcept;
  void MarkRegex(const QString &regex = QString()) noexcept;

  void UnmarkItem() noexcept;
  void UnmarkItems(const QModelIndexList &list) noexcept;
  void UnmarkItemsLocal() noexcept;
  void UnmarkItemsGlobal() noexcept;
  void UnmarkDWIM() noexcept;
  void UnmarkRegex(const QString &regex = QString()) noexcept;

  void GotoFirstItem() noexcept;
  void GotoLastItem() noexcept;
  void GotoItem(const uint &itemNum) noexcept;
  void GotoMiddleItem() noexcept;

  void RenameItem() noexcept;
  void RenameItems(const QStringList &files) noexcept;
  void RenameItemsGlobal() noexcept;
  void RenameItemsLocal() noexcept;
  void RenameDWIM() noexcept;

  void PasteItems(const QString &destDir = QString()) noexcept;

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
  void ToggleHiddenFiles(const bool &state) noexcept;
  inline bool hidden_files_visible() noexcept { return m_hidden_files_shown; }
  void Search(QString searchText = QString(),
              const bool &regex = false) noexcept;
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
  void ShowItemPropertyWidget() noexcept;
  void BulkRename(const QStringList &files) noexcept;

  inline void set_header_stylesheet(const std::string &stylesheet) noexcept {
    m_table_view->horizontalHeader()->setStyleSheet(QString::fromStdString(stylesheet));
  }

  inline std::string get_header_stylesheet() noexcept {
    return m_table_view->horizontalHeader()->styleSheet().toStdString();
  }

  inline void ToggleHeaders(const bool &state) noexcept {
    m_header_visible = state;
    m_table_view->horizontalHeader()->setVisible(state);
  }

  inline QStringList get_header_columns() noexcept {
    return m_model->get_columns();
  }

  inline void ToggleHeaders() noexcept {
    m_header_visible = !m_header_visible;
    m_table_view->horizontalHeader()->setVisible(m_header_visible);
  }

  inline void SetCycle(const bool &state) noexcept { m_cycle_item = state; }
  inline bool get_cycle() noexcept { return m_cycle_item; }
  inline void ToggleCycle() noexcept { m_cycle_item = !m_cycle_item; }

  inline bool get_headers_visible() noexcept { return m_header_visible; }

  void ToggleVisualLine() noexcept;
  void ToggleVisualLine(const bool &state) noexcept;

  void ToggleMouseScroll() noexcept;
  void ToggleMouseScroll(const bool &state) noexcept;

  inline bool has_selection() noexcept {
    return m_table_view->selectionModel()->hasSelection();
  }

  inline void SetBulkRenameThreshold(const unsigned int &threshold) noexcept {
    m_bulk_rename_threshold = threshold;
  }

  inline int Bulk_rename_threshold() noexcept {
    return m_bulk_rename_threshold;
  }

  // Editor to use when bulk renaming
  inline void SetBulkRenameEditor(const QString &editor) noexcept {
    m_bulk_rename_editor = editor;
  }

  // Returns the editor used for bulk renaming (if set)
  inline std::string Bulk_rename_editor() noexcept {
    return m_bulk_rename_editor.toStdString();
  }

  // Terminal for use from within `m_file_panel`
  inline void SetTerminal(const QString &terminal) noexcept {
    m_terminal = terminal;
  }

  // Whether to use the terminal editor when bulk renaming
  inline void SetBulkRenameWithTerminal(const bool &state) noexcept {
    m_bulk_rename_with_terminal = state;
  }

  // Whether to use the terminal editor when bulk renaming
  inline bool Is_bulk_rename_with_terminal() noexcept {
    return m_bulk_rename_with_terminal;
  }

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

  void copy_to() noexcept;
  void move_to() noexcept;
  void link_to() noexcept;
  void goto_symlink_target() noexcept;
  void LinkItems(const QStringList &files, const QString &target_path,
                 const bool &hard_link = false) noexcept;
  void NextPage() noexcept;
  void PrevPage() noexcept;

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
  void SelectFirstItem() noexcept;
  void initContextMenu() noexcept;
  void handleItemDoubleClicked(const QModelIndex &index) noexcept;
  void initSignalsSlots() noexcept;
  QVBoxLayout *m_layout = new QVBoxLayout();
  TableView *m_table_view = new TableView();
  FileSystemModel *m_model = new FileSystemModel();
  QString m_current_dir = "~";
  bool m_hidden_files_shown = false;
  bool m_hidden_files_just_toggled = false;
  bool m_dot_dot_shown = false;
  QAction *m_context_action_open = nullptr;
  QMenu *m_context_action_open_with = nullptr;
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
  FileWorker::FileOPType m_file_op_type = FileWorker::FileOPType::COPY;
  QString m_terminal;
  QStringList m_terminal_args;
  Inputbar *m_inputbar;
  Statusbar *m_statusbar;
  TaskManager *m_task_manager = nullptr;
  HookManager *m_hook_manager = nullptr;
  // This is used to store the filenames that are marked for cutting or copying.
  QStringList m_register_files_list;
  QPoint m_drag_start_position;
  bool m_cycle_item = true;
  bool m_visual_line_mode = false;
  bool m_scroll_action = true;
  int m_bulk_rename_threshold = 5;
  QString m_bulk_rename_editor = "nvim";
  QModelIndex m_visual_start_index;
  int m_file_name_column_index = -1;
  QColor m_current_background, m_current_foreground;
  QModelIndex m_highlight_index;
  QString m_highlight_text;
  QHash<QString, QString> m_default_applications_hash;
  bool m_bulk_rename_with_terminal = true;
  QString m_previous_dir_path;
  bool m_header_visible = true;
  QItemSelectionModel *m_selection_model;
  MimeUtils *m_mime_utils = new MimeUtils();
  QMenu *m_context_menu_folder = new QMenu(this),
        *m_context_menu_file = new QMenu(this);
};
