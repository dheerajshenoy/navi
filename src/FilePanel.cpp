#include "FilePanel.hpp"
#include "Navi.hpp"

FilePanel::FilePanel(Inputbar *inputBar, Statusbar *statusBar, HookManager *hm,
                     TaskManager *tm, QWidget *parent)
    : QWidget(parent), m_inputbar(inputBar), m_statusbar(statusBar),
    m_task_manager(tm), m_hook_manager(hm) {
    this->setLayout(m_layout);
    m_layout->addWidget(m_table_view);
    m_table_view->setModel(m_model);
    m_mime_utils->loadDefaults();
    m_selection_model = m_table_view->selectionModel();

    setContentsMargins(0, 0, 0, 0);

    connect(m_table_view, &TableView::cursorPositionChanged, this,
            [&](const int &row) {
            m_model->set_cursor_row(row);
            });

    initSignalsSlots();
    initContextMenu();
    m_file_name_column_index = m_model->fileNameColumnIndex();
    m_table_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    setAcceptDrops(true);

    m_context_menu_folder->addAction(m_context_action_open);
    m_context_menu_folder->addAction(m_context_action_cut);
    m_context_menu_folder->addAction(m_context_action_copy);
    m_context_menu_folder->addAction(m_context_action_paste);
    m_context_menu_folder->addAction(m_context_action_delete);
    m_context_menu_folder->addAction(m_context_action_rename);
    m_context_menu_folder->addAction(m_context_action_trash);
    m_context_menu_folder->addAction(m_context_action_properties);

    m_context_menu_file->addAction(m_context_action_open);
    m_context_menu_file->addMenu(m_context_action_open_with);
    m_context_menu_file->addAction(m_context_action_cut);
    m_context_menu_file->addAction(m_context_action_copy);
    m_context_menu_file->addAction(m_context_action_paste);
    m_context_menu_file->addAction(m_context_action_delete);
    m_context_menu_file->addAction(m_context_action_rename);
    m_context_menu_file->addAction(m_context_action_trash);
    m_context_menu_file->addAction(m_context_action_properties);

    this->show();
}

void FilePanel::ResetFilter() noexcept {
    m_model->setNameFilters(QStringList() << "*");
    m_hook_manager->triggerHook("filter_mode_off");
    ForceUpdate();
}

void FilePanel::initContextMenu() noexcept {

    m_context_action_open = new QAction("Open");
    m_context_action_open_with = new QMenu("Open With");
    m_context_action_cut = new QAction("Cut");
    m_context_action_copy = new QAction("Copy");
    m_context_action_paste = new QAction("Paste");
    m_context_action_delete = new QAction("Delete");
    m_context_action_rename = new QAction("Rename");
    m_context_action_trash = new QAction("Trash");
    m_context_action_properties = new QAction("Properties");

    connect(m_context_action_rename, &QAction::triggered, this,
            [&]() { RenameDWIM(); });

    connect(m_context_action_cut, &QAction::triggered, this,
            [&]() { CutDWIM(); });

    connect(m_context_action_open, &QAction::triggered, this,
            [&]() { SelectItem(); });

    connect(m_context_action_copy, &QAction::triggered, this,
            [&]() { CopyDWIM(); });

    connect(m_context_action_paste, &QAction::triggered, this,
            [&]() { PasteItems(); });

    connect(m_context_action_delete, &QAction::triggered, this,
            [&]() { DeleteDWIM(); });

    connect(m_context_action_trash, &QAction::triggered, this,
            [&]() { TrashDWIM(); });

    connect(m_context_action_properties, &QAction::triggered, this,
            [&]() { ShowItemPropertyWidget(); });
}

void FilePanel::initSignalsSlots() noexcept {

    connect(m_table_view, &TableView::dragRequested, this,
            &FilePanel::dragRequested);

    connect(m_model, &FileSystemModel::directoryLoadProgress, this,
            [&](const int &progress) {});

    connect(this, &FilePanel::dropCopyRequested, this,
            [&](const QStringList &sourceFilePaths) {
            m_file_op_type = FileWorker::FileOPType::COPY;
            m_register_files_list = sourceFilePaths;
            PasteItems();
            });

    connect(this, &FilePanel::dropCutRequested, this,
            [&](const QStringList &sourceFilePaths) {
            m_file_op_type = FileWorker::FileOPType::CUT;
            m_register_files_list = sourceFilePaths;
            PasteItems();
            });

    connect(m_table_view, &QTableView::clicked, [&](const QModelIndex &index) {
        auto endIndex = m_table_view->currentIndex();
        m_table_view->setCurrentIndex(index);

        if (m_visual_line_mode) {
            // Update selection to include the new index
            QItemSelection selection(m_visual_start_index, endIndex);
            m_selection_model->select(selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                      QItemSelectionModel::SelectionFlag::Rows);
            m_selection_model->setCurrentIndex(endIndex, QItemSelectionModel::NoUpdate);
            m_table_view->viewport()->update();
            /*m_table_view->viewport()->update();*/
        }

        m_table_view->viewport()->update();
    });

    connect(m_table_view, &QTableView::doubleClicked, this,
            &FilePanel::handleItemDoubleClicked);

    connect(m_selection_model, &QItemSelectionModel::currentChanged,
            this, [this](const QModelIndex &current, const QModelIndex &previous) {
            UNUSED(previous);
            emit currentItemChanged(m_model->data(current,
                                                  static_cast<int>(FileSystemModel::Role::FilePath)).toString());
            });

    connect(this, &FilePanel::dropCopyRequested, this,
            &FilePanel::DropCopyRequested);

    connect(this, &FilePanel::dropCutRequested, this,
            &FilePanel::DropCutRequested);

    connect(m_model, &FileSystemModel::directoryLoaded, this,
            [&](const int &rowCount) {
            m_statusbar->SetNumItems(rowCount);
            if (rowCount == 0) {
            m_table_view->clearSelection();
            return;
            }
            if (m_hidden_files_just_toggled) {
            if (!(m_highlight_text.isEmpty() ||
            m_highlight_text.isNull())) {
            QModelIndex index =
            m_model->getIndexFromBaseName(m_highlight_text);
            if (index.isValid()) {
            m_table_view->setCurrentIndex(index);
            m_table_view->scrollTo(index);
            }
            }
            m_hidden_files_just_toggled = false;
            } else {

            if (!(m_highlight_text.isEmpty())) {
            auto row = m_model->getRowFromBaseName(m_highlight_text);
            if (row == -1) {
            if (m_model->rowCount() > 0)
            row = 0;
            }
            QModelIndex index = m_model->index(row, 0);
            m_table_view->setCurrentIndex(index);
            m_highlight_text.clear();
            return;
            }

            auto highlight_row = m_highlight_index.row();

            if (highlight_row < rowCount) {
            QModelIndex index = m_highlight_index;
            m_table_view->setCurrentIndex(index);
            } else if (highlight_row > 0) {
            QModelIndex index = m_model->index(m_highlight_index.row() - 1, 0);
            m_table_view->setCurrentIndex(index);
            } else if (highlight_row == -1) {
            QModelIndex index = m_model->index(0, 0);
            m_table_view->setCurrentIndex(index);
            }
            else
            m_table_view->clearSelection();
            }

            QModelIndex current = m_selection_model->currentIndex();
            if (current.isValid()) {
            emit currentItemChanged(m_model->data(current,
                                                  static_cast<int>(FileSystemModel::Role::FilePath))
                                    .toString()
                                    );
            }
            m_hook_manager->triggerHook("directory_loaded");
            });

    // TODO: Partial reload of directory
    connect(m_model->getFileSystemWatcher(),
            &QFileSystemWatcher::directoryChanged, m_model,
            [&](const QString &path) {
            m_highlight_index = m_table_view->currentIndex();
            m_model->loadDirectory(path);
            });
}

void FilePanel::DropCopyRequested(const QStringList &sourcePaths) noexcept {
    QString destDir = m_current_dir;

    QThread *thread = new QThread(this);
    FileWorker *worker = new FileWorker(sourcePaths, destDir, FileWorker::FileOPType::COPY, m_task_manager);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &FileWorker::performOperation);

    connect(worker, &FileWorker::finished, this,
            [&]() { emit fileOperationDone(true); });

    connect(worker, &FileWorker::error, this, [&](const QString &reason) {
        emit fileOperationDone(false, reason);
    });
    // connect(worker, &FileWorker::canceled, this,
    // &FileCopyWidget::handleCanceled);

    // TODO: Add Progress
    // connect(worker, &FileCopyWorker::progress, this,
    // &FileCopyWidget::updateProgress);

    connect(worker, &FileWorker::finished, thread, &QThread::quit);
    connect(worker, &FileWorker::finished, worker, &FileWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

void FilePanel::DropCutRequested(const QStringList &sourcePaths) noexcept {

    QString destDir = m_current_dir;

    QThread *thread = new QThread(this);
    FileWorker *worker = new FileWorker(sourcePaths, destDir, FileWorker::FileOPType::CUT, m_task_manager);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &FileWorker::performOperation);

    connect(worker, &FileWorker::finished, this,
            [&]() { emit fileOperationDone(true); });

    connect(worker, &FileWorker::error, this, [&](const QString &reason) {
        emit fileOperationDone(false, reason);
    });

    // TODO: File worker cancelled
    // connect(worker, &FileWorker::canceled, this,
    // &FileCopyWidget::handleCanceled);

    connect(worker, &FileWorker::finished, thread, &QThread::quit);
    connect(worker, &FileWorker::finished, worker, &FileWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

QString FilePanel::getCurrentItem() noexcept {
    return m_model->data(m_table_view->currentIndex(),
                         static_cast<int>(FileSystemModel::Role::FilePath))
    .toString();
}

QString FilePanel::getCurrentItemFileName() noexcept {
    return m_model->data(m_table_view->currentIndex(),
                         static_cast<int>(FileSystemModel::Role::FileName)).toString();
}

void FilePanel::handleItemDoubleClicked(const QModelIndex &index) noexcept {
    selectHelper(index, true);
}

void FilePanel::setCurrentDir(QString path, const bool &SelectFirstItem) noexcept {

    if (path.isEmpty())
        return;

    if (path.contains("~")) {
        path = path.replace("~", QDir::homePath());
    }

    QFileInfo fileinfo(path);
    if (fileinfo.exists()) {

        if (fileinfo.isFile()) {
            path = fileinfo.absolutePath();
        }

        m_model->setRootPath(path);
        m_previous_dir_path = m_current_dir;
        m_current_dir = path;

        if (SelectFirstItem) {
            m_table_view->setCurrentIndex(m_model->index(0, 0));
        }

        m_search_new_directory = true;
        if (m_model->rowCount() > 0) {
            emit currentItemChanged(m_model->data(m_table_view->currentIndex(),
                                                  static_cast<int>(FileSystemModel::Role::FilePath)).toString());
        }

        emit afterDirChange(m_current_dir);
    }
}


QString FilePanel::getCurrentDir() noexcept { return m_current_dir; }

FilePanel::~FilePanel() {}

///////////////////////////
// Interactive Functions //
///////////////////////////

void FilePanel::NextItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    if (!currentIndex.isValid())
        return;
    int nextRow = currentIndex.row() + 1;

    // Loop to start
    if (m_cycle_item && nextRow >= m_model->rowCount(m_table_view->rootIndex()))
        nextRow = 0;

    QModelIndex oldIndex = m_table_view->currentIndex();
    QModelIndex nextIndex = m_model->index(nextRow, 0);
    if (nextIndex.isValid()) {
        m_table_view->setCurrentIndex(nextIndex);
        m_table_view->scrollTo(nextIndex);

        if (m_visual_line_mode) {
            // Update selection to include the new index
            QItemSelection selection(m_visual_start_index, nextIndex);
            m_selection_model->select(selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                      QItemSelectionModel::SelectionFlag::Rows);
            m_selection_model->setCurrentIndex(nextIndex, QItemSelectionModel::NoUpdate);
            m_table_view->viewport()->update();
            /*m_table_view->viewport()->update();*/
        }
    }
}

void FilePanel::PrevItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    if (!currentIndex.isValid())
        return;
    int prevRow = currentIndex.row() - 1;

    // // Loop
    if (m_cycle_item && prevRow < 0)
        prevRow = m_model->rowCount(m_table_view->rootIndex()) - 1;

    QModelIndex prevIndex = m_model->index(prevRow, 0);

    if (prevIndex.isValid()) {
        m_table_view->setCurrentIndex(prevIndex);
        m_table_view->scrollTo(prevIndex);
        if (m_visual_line_mode) {
            // Update selection to include the new index
            QItemSelection selection(m_visual_start_index, prevIndex);
            m_selection_model->select(
                selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                QItemSelectionModel::SelectionFlag::Rows);
            m_selection_model->setCurrentIndex(
                prevIndex, QItemSelectionModel::NoUpdate);
            m_table_view->viewport()->update();
        }
    }
}

void FilePanel::selectHelper(const QModelIndex &index,
                             const bool selectFirst) noexcept {
    QString filepath = index.data(static_cast<int>(FileSystemModel::Role::FilePath)).toString();
    if (m_model->isDir(index)) {
        setCurrentDir(filepath, selectFirst);

    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
    }

    m_hook_manager->triggerHook("item_select");
}

void FilePanel::SelectItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    if (m_visual_line_mode)
        m_visual_line_mode = false;
    selectHelper(currentIndex, true);
}

void FilePanel::SelectItemHavingString(const QString &itemName) noexcept {
    QModelIndex currentIndex = m_model->getIndexFromString(itemName);
    selectHelper(currentIndex, true);
}

void FilePanel::highlightIndex(const QModelIndex &index) noexcept {
    m_table_view->setCurrentIndex(index);
}

void FilePanel::HighlightItem(const QString &itemName) noexcept {
    QModelIndex index = m_model->getIndexFromString(itemName);
    if (index.isValid())
        m_table_view->setCurrentIndex(index);
}

void FilePanel::HighlightItemWithBaseName(const QString &baseName) noexcept {
    QModelIndex index = m_model->getIndexFromBaseName(baseName);
    if (index.isValid())
        m_table_view->setCurrentIndex(index);
}


void FilePanel::UpDirectory() noexcept {
    QString old_dir = m_current_dir;
    QDir currentDir(old_dir);

    if (currentDir.cdUp()) {
        if (m_visual_line_mode)
            m_visual_line_mode = false;
        setCurrentDir(currentDir.absolutePath());
        QModelIndex oldDirIndex = m_model->index(old_dir);
        if (oldDirIndex.isValid()) {
            m_table_view->setCurrentIndex(oldDirIndex);
            m_table_view->scrollTo(oldDirIndex);
        }
        emit afterDirChange(m_current_dir);
    }

    m_hook_manager->triggerHook("directory_up");
}

void FilePanel::ToggleMarkItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    if (m_model->data(currentIndex, static_cast<int>(FileSystemModel::Role::Marked)).toBool()) {
        m_model->setData(currentIndex, false, static_cast<int>(FileSystemModel::Role::Marked));
    } else {
        m_model->setData(currentIndex, true, static_cast<int>(FileSystemModel::Role::Marked));
    }
}

void FilePanel::ToggleMarkDWIM() noexcept {
    if (m_selection_model->hasSelection()) {
        auto indexes = m_selection_model->selectedRows();
        for (const auto &index : indexes) {
            if (m_model->data(index, static_cast<int>(FileSystemModel::Role::Marked)).toBool()) {
                m_model->setData(index, false, static_cast<int>(FileSystemModel::Role::Marked));
            } else {
                m_model->setData(index, true, static_cast<int>(FileSystemModel::Role::Marked));
            }
        }
    } else {
        ToggleMarkItem();
    }
    ToggleVisualLine(false);
}

void FilePanel::MarkItem() noexcept {
    m_model->setData(m_table_view->currentIndex(),
                     true,
                     static_cast<int>(FileSystemModel::Role::Marked));
}

void FilePanel::MarkItems(const QModelIndexList &list) noexcept {
    for (auto index : list) {
        m_model->setData(index, true, static_cast<int>(FileSystemModel::Role::Marked));
    }
    ToggleVisualLine(false);
}

void FilePanel::MarkDWIM() noexcept {
    if (m_selection_model->hasSelection()) {
        auto selected_rows = m_selection_model->selectedRows();
        selected_rows.remove(selected_rows.indexOf(m_table_view->currentIndex()));
        MarkItems(selected_rows);
    } else {
        MarkItem();
    }
    ToggleVisualLine(false);
}

void FilePanel::MarkRegex(const QString &regex) noexcept {
    if (regex.isEmpty() || regex.isNull()) {
        QString searchExpression = QInputDialog::getText(this, "Mark Regex", "Regex pattern to mark items with");
        m_search_index_list = m_model->match(m_model->index(0, 0), Qt::DisplayRole,
                                             searchExpression, -1,
                                             Qt::MatchRegularExpression);
    } else {
        m_search_index_list = m_model->match(m_model->index(0, 0), Qt::DisplayRole,
                                             regex, -1,
                                             Qt::MatchRegularExpression);
    }
    if (m_search_index_list.isEmpty())
        return;

    MarkItems(m_search_index_list);

    ToggleVisualLine(false);
}

void FilePanel::UnmarkDWIM() noexcept {
    if (m_selection_model->hasSelection()) {
        UnmarkItems(m_selection_model->selectedRows());
    } else {
        MarkItem();
    }
    ToggleVisualLine(false);
}

void FilePanel::UnmarkItems(const QModelIndexList &list) noexcept {
    for (auto index : list) {
        m_model->setData(index, false, static_cast<int>(FileSystemModel::Role::Marked));
    }
    ToggleVisualLine(false);
}

void FilePanel::UnmarkItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    m_model->removeMarkedFile(currentIndex);
}

void FilePanel::UnmarkItemsGlobal() noexcept {
    if (m_model->hasMarks()) {
        auto confirm = QMessageBox::question(this, "Unmark Items Global",
                                             QString("Do you want to unmark %1 global items").arg(m_model->getMarkedFilesCount()));

        if (confirm == QMessageBox::StandardButton::Yes)
            m_model->removeMarkedFiles();
        else
            m_statusbar->Message("Unmark cancelled");
    } else
    m_statusbar->Message("No marks found");
}

void FilePanel::UnmarkItemsLocal() noexcept {
    if (m_model->hasMarksLocal()) {
        m_model->clearMarkedFilesListLocal();
    } else
    m_statusbar->Message("No local marks found", MessageType::WARNING);
}

void FilePanel::UnmarkRegex(const QString &regex) noexcept {
    if (regex.isEmpty() || regex.isNull()) {
        QString searchExpression = QInputDialog::getText(this, "Unmark Regex", "Regex pattern to unmark items");
        m_search_index_list = m_model->match(m_model->index(0, 0), Qt::DisplayRole,
                                             searchExpression, -1,
                                             Qt::MatchRegularExpression);
    } else {
        m_search_index_list = m_model->match(m_model->index(0, 0), Qt::DisplayRole,
                                             regex, -1,
                                             Qt::MatchRegularExpression);
    }
    if (m_search_index_list.isEmpty())
        return;

    UnmarkItems(m_search_index_list);

    ToggleVisualLine(false);
}

void FilePanel::GotoFirstItem() noexcept {
    QModelIndex index = m_model->index(0, 0);
    m_table_view->setCurrentIndex(index);


    if (m_visual_line_mode) {
        // Update selection to include the new index
        QItemSelection selection(m_visual_start_index, index);
        m_selection_model->select(
            selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
            QItemSelectionModel::SelectionFlag::Rows);
        m_selection_model->setCurrentIndex(
            index, QItemSelectionModel::NoUpdate);
    }
}

void FilePanel::GotoLastItem() noexcept {
    QModelIndex index =
        m_model->index(m_model->rowCount(m_table_view->rootIndex()) - 1, 0);
    m_table_view->setCurrentIndex(index);

    if (m_visual_line_mode) {
        // Update selection to include the new index
        QItemSelection selection(m_visual_start_index, index);
        m_selection_model->select(selection,
                                  QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        m_selection_model->setCurrentIndex(index,
                                           QItemSelectionModel::NoUpdate);
    }
}

void FilePanel::GotoItem(const uint &itemNum) noexcept {
    if (itemNum >= 0 &&
        itemNum <= m_model->rowCount(m_table_view->rootIndex()) - 1) {
        QModelIndex index = m_model->index(itemNum, 0);
        m_table_view->setCurrentIndex(index);

        if (m_visual_line_mode) {
            // Update selection to include the new index
            QItemSelection selection(m_visual_start_index, index);
            m_selection_model->select(
                selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                QItemSelectionModel::SelectionFlag::Rows);
            m_selection_model->setCurrentIndex(
                index, QItemSelectionModel::NoUpdate);
        }
    }
}

void FilePanel::GotoMiddleItem() noexcept {
    auto index = m_table_view->scroll_to_middle();

    if (m_visual_line_mode) {
        // Update selection to include the new index
        QItemSelection selection(m_visual_start_index, index);
        m_selection_model->select(selection,
                                  QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        m_selection_model->setCurrentIndex(index,
                                           QItemSelectionModel::NoUpdate);
    }
}

void FilePanel::TrashItem() noexcept {
    const QString &filePath = getCurrentItem();

    bool state = QFile::moveToTrash(filePath);
    if (state) {
        m_model->removeMarkedFile(filePath);
        m_statusbar->Message(QString("Trashed item %1 successfully!")
                             .arg(QFileInfo(filePath).fileName()));
    } else
    m_statusbar->Message(QString("Error trashing item %1 successfully!")
                         .arg(QFileInfo(filePath).fileName()));
}

void FilePanel::TrashItems(const QStringList &files) noexcept {
    bool check = true;
    for (const auto &filePath : files) {
        if (check) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Trash Items");
            msgBox.setText(QString("Do you want to delete %1 and all of it's subitems ?").arg(filePath));
            auto yes_btn = msgBox.addButton("Yes", QMessageBox::ButtonRole::YesRole);
            auto no_btn = msgBox.addButton("No", QMessageBox::ButtonRole::NoRole);
            auto yes_to_all_btn = msgBox.addButton("Yes to All", QMessageBox::ButtonRole::ApplyRole);
            auto no_to_all_btn = msgBox.addButton("No to All", QMessageBox::ButtonRole::RejectRole);
            msgBox.setDefaultButton(no_btn);

            msgBox.exec();

            auto inputConfirm = msgBox.clickedButton();

            if (inputConfirm == yes_btn) {
                if (QFile::moveToTrash(filePath)) {
                    m_model->removeMarkedFile(filePath);
                    m_statusbar->Message(QString("Trashed item %1 successfully!")
                                         .arg(QFileInfo(filePath).fileName()));
                } else
                m_statusbar->Message(QString("Error trashing item %1 successfully!")
                                     .arg(QFileInfo(filePath).fileName()));
            } else if (inputConfirm == yes_to_all_btn) {
                check = false;
                if (QFile::moveToTrash(filePath)) {
                    m_model->removeMarkedFile(filePath);
                    m_statusbar->Message(QString("Trashed item %1 successfully!")
                                         .arg(QFileInfo(filePath).fileName()));
                } else
                m_statusbar->Message(QString("Error trashing item %1 successfully!")
                                     .arg(QFileInfo(filePath).fileName()));
            } else if (inputConfirm == no_to_all_btn) {
                m_statusbar->Message("Trash operation cancelled");
                return;
            } else
            continue;

        } else {
            if (QFile::moveToTrash(filePath)) {
                m_model->removeMarkedFile(filePath);
                m_statusbar->Message(QString("Trashed item %1 successfully!")
                                     .arg(QFileInfo(filePath).fileName()));
            } else
            m_statusbar->Message(QString("Error trashing item %1 successfully!")
                                 .arg(QFileInfo(filePath).fileName()));
        }
    }
}

void FilePanel::TrashDWIM() noexcept {
    auto localMarks = m_model->getMarkedFilesLocal();

    if (localMarks.isEmpty()) {
        TrashItem();
        return;
    }

    TrashItems(localMarks);
}

void FilePanel::TrashItemsLocal() noexcept {
    auto localMarkFiles = m_model->getMarkedFilesLocal();
    if (localMarkFiles.size() == 0) {
        m_statusbar->Message("No local marks found", MessageType::WARNING);
        return;
    }

    for (const auto &file : localMarkFiles) {
        if (QFile::moveToTrash(file))
            m_statusbar->Message("Error trashing %1", MessageType::ERROR);
        else
            m_statusbar->Message("Trashed %1 successfully");
    }
}

void FilePanel::TrashItemsGlobal() noexcept {

    auto markFiles = m_model->getMarkedFiles();
    if (markFiles.size() == 0) {
        m_statusbar->Message("No marks found", MessageType::WARNING);
        return;
    }

    for (const auto &file : markFiles) {
        if (QFile::moveToTrash(file))
            m_statusbar->Message("Error trashing %1", MessageType::ERROR);
        else
            m_statusbar->Message("Trashed %1 successfully");
    }
}

void FilePanel::Filters(const QString &filterString) noexcept {
    QStringList filterStringList = filterString.split(" ");
    if (filterStringList.isEmpty())
        filterStringList = {filterString};
    m_model->setNameFilters(filterStringList);
    m_hook_manager->triggerHook("filter_mode_on");
    ForceUpdate();
}

void FilePanel::RenameItem() noexcept {
    // current selection single rename
    const QString &oldName = getCurrentItem();
    QFileInfo fileInfo(oldName);
    const QString &oldFileName = fileInfo.fileName();
    QString newFileName;

    if (fileInfo.isFile()) {
        newFileName = utils::getInput(this, "Rename Item",
                                      QString("Rename (%1)").arg(oldFileName),
                                      oldFileName, fileInfo.baseName());
    }
    else if (fileInfo.isDir())
        newFileName = utils::getInput(this, QString("Rename (%1)").arg(oldFileName),
                                      oldFileName,
                                      oldFileName);

    // If user cancels or the new file name is empty
    if (newFileName.isEmpty() || newFileName.isNull()) {
        m_statusbar->Message("Operation Cancelled", MessageType::WARNING);
        return;
    }

    if (newFileName == oldName) {
        m_statusbar->Message("File names are same", MessageType::WARNING);
        return;
    }

    bool state =
        QFile::rename(oldName, m_current_dir + QDir::separator() + newFileName);
    if (state) {
        m_statusbar->Message("Rename successful");
        m_model->removeMarkedFile(oldName);
    }

    m_highlight_text = newFileName;
}

void FilePanel::BulkRename(const QStringList &filePaths) noexcept {
    QTemporaryFile tempFile;

    tempFile.setFileTemplate("/tmp/Navi__Bulk_Rename_List__XXXXXXXXXX");
    tempFile.setAutoRemove(false);

    if (!tempFile.open()) {
        m_statusbar->Message("Error creating temporary file", MessageType::ERROR,
                             5);
        return;
    }

    // Step 2: Write original filenames to temporary file with placeholders for
    // new names
    QTextStream out(&tempFile);

    out << "# This is a comment line\n";
    out << "# Add your new file names to the right of the -> symbol\n";
    out << "# Just add the file name without the path\n";
    out << "# Example: /path/to/file/filename.png -> new_filename.png\n";
    out << "# Once done, please save and quit the file\n";
    out << "# The files will hopefully be renamed\n\n";

    for (const QString &filePath : filePaths) {
        out << filePath << " -> "
            << "\n"; // Format: original_path -> new_name_placeholder
    }

    tempFile.close();

    // Step 3: Open the temporary file in the default text editor
    QProcess editor;

    editor.startDetached();
    if (m_bulk_rename_with_terminal) {
        editor.start(m_terminal,
                     QStringList()
                     << m_bulk_rename_editor << tempFile.fileName());
    } else {
        editor.start(m_bulk_rename_editor, QStringList() << tempFile.fileName());
    }
    editor.waitForFinished();

    // Step 4: Reopen the file to read user inputs
    if (!tempFile.open()) {
        m_statusbar->Message("Could not reopen temporary file.", MessageType::ERROR,
                             5);
        return;
    }

    // Step 5: Parse each line and rename files
    QTextStream in(&tempFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("#"))
            continue; // skip "comments"
        QStringList parts = line.split("->", Qt::SkipEmptyParts);
        if (parts.size() == 2) {
            QString originalPath = parts[0].trimmed();
            QString newName = parts[1].trimmed();
            if (!newName.isEmpty()) {
                QString newFilePath =
                    QFileInfo(originalPath).absolutePath() + "/" + newName;
                if (!QFile::rename(originalPath, newFilePath)) {
                    m_statusbar->Message(QString("Failed to rename %1").arg(originalPath),
                                         MessageType::WARNING);
                } else {
                    m_model->removeMarkedFile(originalPath);
                    m_statusbar->Message("Rename successful");
                }
            }
        }
    }
    tempFile.close();
    tempFile.remove();
}

void FilePanel::RenameItemsGlobal() noexcept {
    if (m_model->hasMarks()) {
        const auto &markedFiles = m_model->getMarkedFiles();
        if (markedFiles.size() > 1) {
            BulkRename(markedFiles);
        } else {
            QFileInfo fileInfo(markedFiles.at(0));
            const QString &oldFilePath = fileInfo.filePath();
            const QString &oldName = fileInfo.fileName();
            QString newFileName;
            if (fileInfo.isFile())
                newFileName = m_inputbar->getInput(QString("Rename (%1)").arg(oldName),
                                                   oldName, fileInfo.baseName());
            else if (fileInfo.isDir())
                newFileName = m_inputbar->getInput(QString("Rename (%1)").arg(oldName),
                                                   oldName, oldName);

            // If user cancels or the new file name is empty
            if (newFileName.isEmpty() || newFileName.isNull()) {
                m_statusbar->Message("Operation Cancelled", MessageType::WARNING);
                return;
            }

            if (newFileName == oldName) {
                m_statusbar->Message("Filename is the same.", MessageType::WARNING);
                return;
            }

            bool state = QFile::rename(
                oldFilePath, m_current_dir + QDir::separator() + newFileName);
            if (state) {
                m_model->removeMarkedFile(oldFilePath);
            }
            return;
        }
    }
}

void FilePanel::RenameItemsLocal() noexcept {
    if (m_model->hasMarksLocal()) {
        const auto &localMarkedFiles = m_model->getMarkedFilesLocal();
        if (localMarkedFiles.size() > 1) {
            BulkRename(localMarkedFiles);
        } else {
            QFileInfo fileInfo(localMarkedFiles.at(0));
            const QString &oldFilePath = fileInfo.filePath();
            const QString &oldName = fileInfo.fileName();
            QString newFileName;

            if (fileInfo.isFile())
                newFileName = m_inputbar->getInput(QString("Rename (%1)").arg(oldName),
                                                   oldName, fileInfo.baseName());
            else if (fileInfo.isDir())
                newFileName = m_inputbar->getInput(QString("Rename (%1)").arg(oldName),
                                                   oldName, oldName);

            // If user cancels or the new file name is empty
            if (newFileName.isEmpty() || newFileName.isNull()) {
                m_statusbar->Message("Operation Cancelled", MessageType::WARNING);
                return;
            }

            if (newFileName == oldName) {
                m_statusbar->Message("Filename is the same.", MessageType::WARNING);
                return;
            }

            bool state = QFile::rename(
                oldFilePath, m_current_dir + QDir::separator() + newFileName);
            if (state) {
                m_model->removeMarkedFile(oldFilePath);
            }
        }
    }
}

void FilePanel::RenameItems(const QStringList &files) noexcept {

    for (const auto &filepath : files) {

        QFileInfo fileInfo(filepath);
        const QString &oldFilePath = fileInfo.filePath();
        const QString &oldName = fileInfo.fileName();
        QString newFileName;

        if (fileInfo.isFile())
            newFileName = m_inputbar->getInput(QString("Rename (%1)").arg(oldName),
                                               oldName, fileInfo.baseName());
        else if (fileInfo.isDir())
            newFileName = m_inputbar->getInput(QString("Rename (%1)").arg(oldName),
                                               oldName, oldName);

        // If user cancels or the new file name is empty
        if (newFileName.isEmpty() || newFileName.isNull()) {
            m_statusbar->Message("Operation Cancelled", MessageType::WARNING);
            return;
        }

        if (newFileName == oldName) {
            m_statusbar->Message("Filename is the same.", MessageType::WARNING);
            return;
        }

        bool state = QFile::rename(oldFilePath,
                                   m_current_dir + QDir::separator() + newFileName);
        if (state) {
            m_model->removeMarkedFile(oldFilePath);
            m_statusbar->Message(
                QString("Renamed %1 to %2").arg(oldName).arg(newFileName));
        }
    }
}

void FilePanel::RenameDWIM() noexcept {
    // If there are local marks
    auto localMarks = m_model->getMarkedFilesLocal();
    if (localMarks.isEmpty()) {
        RenameItem();
        return;
    }

    if (localMarks.size() > m_bulk_rename_threshold) {
        BulkRename(localMarks);
    } else
    RenameItems(localMarks);
}

void FilePanel::DeleteDWIM() noexcept {
    auto localMarks = m_model->getMarkedFilesLocal();
    // TODO: Fix some wierd behavior
    if (localMarks.isEmpty()) {
        DeleteItem();
        return;
    }
    DeleteItems(localMarks);
}

void FilePanel::DeleteItems(const QStringList &files) noexcept {
    unsigned int itemsDeleted = 0;
    QString inputConfirm = m_inputbar->getInput(QString("Do you want to delete %1 items ? (y/N)").arg(files.size()));
    if (inputConfirm == "n" || inputConfirm.isEmpty() || inputConfirm.isNull()) {
        m_statusbar->Message("Delete operation cancelled");
        return;
    }
    for (const auto &itemPath : files) {
        QFileInfo file(itemPath);
        if (file.isDir()) {
            QDir dir(itemPath);
            if (dir.removeRecursively()) {
                m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
                itemsDeleted++;
                m_model->removeMarkedFile(itemPath);
            } else
            m_statusbar->Message(QString("Unable to delete %1").arg(itemPath), MessageType::ERROR);
        } else {
            QFile file(itemPath);

            if (file.remove()) {
                m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
                m_model->removeMarkedFile(itemPath);
                itemsDeleted++;
            } else
            m_statusbar->Message(QString("Unable to delete %1").arg(itemPath), MessageType::ERROR);
        }
    }

    m_statusbar->Message(QString("%1/%2 items deleted successfully").arg(itemsDeleted).arg(files.size()));
}

void FilePanel::DeleteItem() noexcept {
    QString itemPath = getCurrentItem();
    QModelIndex currentIndex = m_table_view->currentIndex();
    if (m_model->isDir(m_table_view->currentIndex())) {
        QDir dir(itemPath);
        auto inputConfirm = QMessageBox::question(this, "Delete Item", QString("Do you want to delete %1?").arg(itemPath));
        if (inputConfirm == QMessageBox::StandardButton::Yes) {
            dir.removeRecursively();
            m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
            m_model->removeMarkedFile(itemPath);
        } else {
            m_statusbar->Message("Delete operation cancelled");
            return;
        }
    } else {
        auto inputConfirm = QMessageBox::question(this, "Delete Item", QString("Do you want to delete %1?").arg(itemPath));
        if (inputConfirm == QMessageBox::StandardButton::Yes) {
            if (std::filesystem::remove(itemPath.toStdString())) {
                m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
                m_model->removeMarkedFile(itemPath);
            }
            else
            m_statusbar->Message(QString("Error deleting %1!").arg(itemPath));
        } else {
            m_statusbar->Message("Delete operation cancelled");
            return;
        }
    }

    m_highlight_index = currentIndex;
    m_hidden_files_just_toggled = false;
    m_highlight_text.clear();
}

void FilePanel::DeleteItemsGlobal() noexcept {
    unsigned int itemsDeleted = 0;

    if (m_model->hasMarks()) {
        auto markedFiles = m_model->getMarkedFiles();
        QString confirm = m_inputbar->getInput(
            QString("Do you want to delete %1 files ? (y/N)")
            .arg(markedFiles.size()))
            .toLower();
        if (confirm == "n" || confirm.isEmpty() || confirm.isNull()) {
            m_statusbar->Message("Delete operation cancelled");
            return;
        }
        for (const auto &itemPath : markedFiles) {
            QFileInfo file(itemPath);
            if (file.isDir()) {
                QDir dir(itemPath);
                if (dir.removeRecursively()) {
                    m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
                    itemsDeleted++;
                    m_model->removeMarkedFile(itemPath);
                } else
                m_statusbar->Message(QString("Unable to delete %1").arg(itemPath), MessageType::ERROR);
            } else {
                QFile file(itemPath);

                if (file.remove()) {
                    m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
                    m_model->removeMarkedFile(itemPath);
                    itemsDeleted++;
                } else
                m_statusbar->Message(QString("Unable to delete %1").arg(itemPath), MessageType::ERROR);
            }
        }
        m_statusbar->Message(QString("%1/%2 items deleted successfully").arg(itemsDeleted).arg(markedFiles.size()));
    }
}

void FilePanel::DeleteItemsLocal() noexcept {
    unsigned int itemsDeleted = 0;

    if (m_model->hasMarksLocal()) {
        auto markedLocalFiles = m_model->getMarkedFilesLocal();
        QString confirm = m_inputbar->getInput(
            QString("Do you want to delete %1 files ? (y/N)")
            .arg(markedLocalFiles.size()))
            .toLower();
        if (confirm == "n" || confirm.isEmpty() || confirm.isNull()) {
            m_statusbar->Message("Delete operation cancelled");
            return;
        }
        for (const auto &itemPath : markedLocalFiles) {
            QFileInfo file(itemPath);
            if (file.isDir()) {
                QDir dir(itemPath);
                if (dir.removeRecursively()) {
                    m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
                    itemsDeleted++;
                    m_model->removeMarkedFile(itemPath);
                } else
                m_statusbar->Message(QString("Unable to delete %1").arg(itemPath), MessageType::ERROR);
            } else {
                QFile file(itemPath);

                if (file.remove()) {
                    m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
                    m_model->removeMarkedFile(itemPath);
                    itemsDeleted++;
                } else
                m_statusbar->Message(QString("Unable to delete %1").arg(itemPath), MessageType::ERROR);
            }
        }
        m_statusbar->Message(QString("%1/%2 items deleted successfully").arg(itemsDeleted).arg(markedLocalFiles.size()));
    }
}

void FilePanel::ToggleHiddenFiles() noexcept {
    m_hidden_files_shown = !m_hidden_files_shown;
    m_hidden_files_just_toggled = m_hidden_files_shown;
    if (m_hidden_files_shown) {
        m_highlight_index = m_table_view->currentIndex();
        m_model->addDirFilter(QDir::Hidden);
        m_highlight_text = getCurrentItemFileName();
    }
    else {
        m_model->removeDirFilter(QDir::Hidden);
        m_highlight_text = getCurrentItemFileName();
    }
    m_model->loadDirectory(m_current_dir);
}

void FilePanel::ToggleHiddenFiles(const bool &state) noexcept {
    m_hidden_files_shown = m_hidden_files_just_toggled = state;
    if (m_hidden_files_shown) {
        m_highlight_index = m_table_view->currentIndex();
        m_model->addDirFilter(QDir::Hidden);
    }
    else {
        m_model->removeDirFilter(QDir::Hidden);
    }
    m_highlight_text = getCurrentItemFileName();
    m_model->loadDirectory(m_current_dir);
}

void FilePanel::ToggleDotDot() noexcept {
    m_dot_dot_shown = !m_dot_dot_shown;
    if (m_dot_dot_shown)
        m_model->removeDirFilter(QDir::NoDotDot);
    else
        m_model->addDirFilter(QDir::NoDotDot);

    m_highlight_index = m_table_view->currentIndex();
    m_model->loadDirectory(m_current_dir);
}

void FilePanel::Search(QString searchExpression, const bool &regex) noexcept {
    if (searchExpression.isNull() || searchExpression.isEmpty())
        searchExpression =
            m_inputbar->getInput("Search", m_search_text, m_search_text);

    if (regex)
        m_search_index_list = m_model->match(m_model->index(0, 0),
                                             Qt::DisplayRole,
                                             searchExpression,
                                             -1,
                                             Qt::MatchRegularExpression);
    else
        m_search_index_list = m_model->match(m_model->index(0, 0),
                                             Qt::DisplayRole,
                                             searchExpression,
                                             -1,
                                             Qt::MatchContains);
    if (m_search_index_list.isEmpty())
        return;
    m_table_view->setCurrentIndex(m_search_index_list.at(0));
    m_search_index_list_index = 0;
    m_search_new_directory = false;
    m_search_text = searchExpression;
    m_statusbar->SetSearchMatchCount(m_search_index_list.size());
    m_statusbar->SetSearchMatchIndex(m_search_index_list_index);
}

void FilePanel::SearchNext() noexcept {
    // Search if not done already
    if (m_search_new_directory) {
        Search(m_search_text);
    }

    if (m_search_index_list.isEmpty()) {
        m_statusbar->Message("No search match found");
        return;
    }

    m_search_index_list_index++;
    if (m_search_index_list_index > m_search_index_list.size() - 1) {
        m_search_index_list_index = 0;
        m_statusbar->Message("Search reached TOP of the directory");
    }

    m_table_view->setCurrentIndex(m_search_index_list.at(m_search_index_list_index));
    m_statusbar->SetSearchMatchIndex(m_search_index_list_index);
}

void FilePanel::SearchPrev() noexcept {

    // Search if not done already
    if (m_search_new_directory) {
        Search(m_search_text);
    }

    if (m_search_index_list.isEmpty()) {
        m_statusbar->Message("No search match found");
        return;
    }

    m_search_index_list_index--;
    if (m_search_index_list_index < 0) {
        m_search_index_list_index = m_search_index_list.size() - 1;
        m_statusbar->Message("Search reached BOTTOM of the directory");
    }

    m_table_view->setCurrentIndex(m_search_index_list.at(m_search_index_list_index));
    m_statusbar->SetSearchMatchIndex(m_search_index_list_index);
}

void FilePanel::contextMenuEvent(QContextMenuEvent *event) {
    auto sel_size = m_selection_model->selectedRows().count();
    m_context_action_open_with->clear();
    if (sel_size > 1) {
        if (is_current_item_a_file()) {
            m_context_action_open_with->addActions(m_mime_utils->app_actions_for_file(getCurrentItem()));
            m_context_menu_file->exec(event->globalPos());
        } else {
            m_context_menu_folder->exec(event->globalPos());
        }
    } else {
        if (is_current_item_a_file()) {
            m_context_action_open_with->addActions(m_mime_utils->app_actions_for_file(getCurrentItem()));
            m_context_menu_file->exec(event->globalPos());
        } else {
            m_context_menu_folder->exec(event->globalPos());
        }
    }
}

void FilePanel::ForceUpdate() noexcept { setCurrentDir(m_current_dir, true); }

bool FilePanel::SetPermissions(const QString &filePath,
                               const QString &permissionString) noexcept {

    QFile file(filePath);
    QFile::Permissions permissions;

    // Parse owner, group, and others permissions from the string
    int ownerPerm = permissionString[0].digitValue();
    int groupPerm = permissionString[1].digitValue();
    int othersPerm = permissionString[2].digitValue();

    // Owner permissions
    if (ownerPerm & 4)
        permissions |= QFile::ReadOwner;
    if (ownerPerm & 2)
        permissions |= QFile::WriteOwner;
    if (ownerPerm & 1)
        permissions |= QFile::ExeOwner;

    // Group permissions
    if (groupPerm & 4)
        permissions |= QFile::ReadGroup;
    if (groupPerm & 2)
        permissions |= QFile::WriteGroup;
    if (groupPerm & 1)
        permissions |= QFile::ExeGroup;

    // Others permissions
    if (othersPerm & 4)
        permissions |= QFile::ReadOther;
    if (othersPerm & 2)
        permissions |= QFile::WriteOther;
    if (othersPerm & 1)
        permissions |= QFile::ExeOther;

    // Set permissions
    return file.setPermissions(permissions);
}

void FilePanel::ChmodItem() noexcept {
    QString permString = m_inputbar->getInput("Enter permission number: ");
    QRegularExpression permissionRegex(R"((^[0-7]{3}$)");
    QRegularExpressionMatch permissionMatch = permissionRegex.match(permString);

    if (permissionMatch.hasMatch()) {
        QString filePath = getCurrentItem();
        if (SetPermissions(filePath, permString)) {
            m_statusbar->Message(QString("Permission changed for %1 successfully").arg(filePath));
        } else {
            m_statusbar->Message("Error setting permission", MessageType::ERROR);
        }
    } else {
        m_statusbar->Message("Invalid permission number", MessageType::WARNING);
    }
}

void FilePanel::ChmodItemsLocal() noexcept {
    auto localMarkedFiles = m_model->getMarkedFilesLocal();
    if (localMarkedFiles.size() == 0) {
        m_statusbar->Message("No local marks found", MessageType::WARNING);
        return;
    }

    QString permString = m_inputbar->getInput("Enter permission number: ");
    QRegularExpression permissionRegex(R"((^[0-7]{3}$)");
    QRegularExpressionMatch permissionMatch = permissionRegex.match(permString);

    if (permissionMatch.hasMatch()) {
        for (const auto &filePath : localMarkedFiles) {
            if (SetPermissions(filePath, permString)) {
                m_statusbar->Message(QString("Permission changed for %1").arg(filePath));
            } else {
                m_statusbar->Message(QString("Error setting permission for %1").arg(filePath),
                                     MessageType::ERROR);
            }
        }
    } else {
        m_statusbar->Message("Invalid permission number", MessageType::WARNING);
    }
}

void FilePanel::ChmodItemsGlobal() noexcept {
    auto localMarkedFiles = m_model->getMarkedFiles();
    if (localMarkedFiles.size() == 0) {
        m_statusbar->Message("No marks found", MessageType::WARNING);
        return;
    }

    QString permString = m_inputbar->getInput("Enter permission number: ");
    QRegularExpression permissionRegex(R"((^[0-7]{3}$)");
    QRegularExpressionMatch permissionMatch = permissionRegex.match(permString);

    if (permissionMatch.hasMatch()) {
        for (const auto &filePath : localMarkedFiles) {
            if (SetPermissions(filePath, permString)) {
                m_statusbar->Message(
                    QString("Permission changed for %1").arg(filePath));
            } else {
                m_statusbar->Message(
                    QString("Error setting permission for %1").arg(filePath),
                    MessageType::ERROR);
            }
        }
    } else {
        m_statusbar->Message("Invalid permission number", MessageType::WARNING);
    }
}

void FilePanel::PasteItems(const QString &_destDir) noexcept {
    QString destDir = _destDir;
    if (m_register_files_list.isEmpty()) {
        m_statusbar->Message("No files in the register!", MessageType::INFO);
        return;
    }

    if (destDir.isEmpty() || destDir.isNull())
        destDir = m_current_dir;

    const QStringList filesList =
        QStringList(m_register_files_list.cbegin(), m_register_files_list.cend());

    FileWorker *worker = new FileWorker(filesList, destDir, m_file_op_type, m_task_manager);
    worker->performOperation();

    connect(worker, &FileWorker::finished, this, [&, filesList]() {
        if (!filesList.isEmpty())
            m_model->removeMarkedFiles(filesList);
        emit fileOperationDone(true);

        auto itemName = utils::fileName(filesList.last());
        HighlightItemWithBaseName(itemName);
    });

    connect(worker, &FileWorker::error, this, [&](const QString &reason) {
        emit fileOperationDone(false, reason);
    });
}

void FilePanel::CopyDWIM() noexcept {

    auto localMarks = m_model->getMarkedFilesLocal();

    if (localMarks.isEmpty()) {
        CopyItem();
        return;
    }
    m_file_op_type = FileWorker::FileOPType::COPY;
    m_register_files_list = localMarks;
}

void FilePanel::CopyItem() noexcept {
    m_file_op_type = FileWorker::FileOPType::COPY;
    m_register_files_list = {getCurrentItem()};
}

void FilePanel::CopyItemsLocal() noexcept {
    m_file_op_type = FileWorker::FileOPType::COPY;
    m_register_files_list = m_model->getMarkedFilesLocal();
}

void FilePanel::CopyItemsGlobal() noexcept {
    m_file_op_type = FileWorker::FileOPType::COPY;
    m_register_files_list = m_model->getMarkedFiles();
}

void FilePanel::CutDWIM() noexcept {

    auto localMarks = m_model->getMarkedFilesLocal();

    if (localMarks.isEmpty()) {
        CutItem();
        return;
    }

    m_file_op_type = FileWorker::FileOPType::CUT;
    m_register_files_list = localMarks;
}

void FilePanel::CutItem() noexcept {
    m_file_op_type = FileWorker::FileOPType::CUT;
    m_register_files_list = {getCurrentItem()};
}

void FilePanel::CutItemsLocal() noexcept {
    m_file_op_type = FileWorker::FileOPType::CUT;
    m_register_files_list = m_model->getMarkedFilesLocal();
}

void FilePanel::CutItemsGlobal() noexcept {
    m_file_op_type = FileWorker::FileOPType::CUT;
    m_register_files_list = m_model->getMarkedFiles();
}

void FilePanel::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void FilePanel::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void FilePanel::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    QString srcPath = event->mimeData()->urls().at(0).toString();

    if (m_current_dir == srcPath) {
        qDebug() << "Dropped to the same location, ignoring drop event";
        event->ignore();
        return;
    }

    if (mimeData->hasUrls()) {
        QStringList files;
        files.reserve(mimeData->urls().size());

        for (const QUrl &url : mimeData->urls()) {
            QString sourcePath = url.toLocalFile();
            files.append(sourcePath);
        }

        // Move or copy the file
        if (event->proposedAction() == Qt::MoveAction) {
            emit dropCutRequested(files);
        } else if (event->proposedAction() == Qt::CopyAction) {
            emit dropCopyRequested(files);
        }
        event->acceptProposedAction();
    }
}

void FilePanel::startDrag(Qt::DropActions supportedActions) {
    UNUSED(supportedActions);

    QModelIndexList selectedIndexes = m_selection_model->selectedRows();

    if (selectedIndexes.isEmpty())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    if (!m_model)
        return;

    // Collect file paths
    QList<QUrl> urls;
    for (const QModelIndex &index : selectedIndexes) {
        QString filePath = m_model->file_at(index.row()).filePath();
        urls << QUrl::fromLocalFile(filePath);
    }

    mimeData->setUrls(urls);

    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}

void FilePanel::ShowItemPropertyWidget() noexcept {
    FilePropertyWidget prop_widget(getCurrentItem(), this);
    prop_widget.exec();
}

void FilePanel::MarkInverse() noexcept {

    if (!m_model->hasMarksLocal())
        return;

    auto nitems = m_model->rowCount();
    for (int i = 0; i < nitems; i++) {
        QModelIndex index = m_model->index(i, 0);
        bool isMarked =
            m_model->data(index, static_cast<int>(FileSystemModel::Role::Marked)).toBool();
        if (isMarked) {
            m_model->setData(index, false, static_cast<int>(FileSystemModel::Role::Marked));
        } else
        m_model->setData(index, true, static_cast<int>(FileSystemModel::Role::Marked));
    }
}

void FilePanel::MarkAllItems() noexcept {
    auto nitems = m_model->rowCount();
    for (int i = 0; i < nitems; i++) {
        QModelIndex index = m_model->index(i, 0);
        m_model->setData(index, true, static_cast<int>(FileSystemModel::Role::Marked));
    }
}

void FilePanel::NewFolder(const QStringList &folderNames) noexcept {
    QDir dir;

    if (folderNames.isEmpty()) {
        QString folderName = m_inputbar->getInput("Enter directory name");
        if (folderName.isEmpty() || folderName.isNull()) {
            m_statusbar->Message(QString("Directory name cannot be empty"),
                                 MessageType::WARNING);
            return;
        }
        dir.setPath(m_current_dir + QDir::separator() + folderName);
        if (dir.mkpath(m_current_dir + QDir::separator() + folderName)) {
            m_statusbar->Message(QString("Directory %1 created").arg(folderName));
            m_highlight_text = folderName;
        }
        else
        m_statusbar->Message(QString("Error creating %1").arg(folderName),
                             MessageType::ERROR, 5);
    } else {
        for (const auto &folderName : folderNames) {
            dir.setPath(m_current_dir + QDir::separator() + folderName);
            if (dir.mkpath(m_current_dir + QDir::separator() + folderName))
                m_statusbar->Message(QString("Directory %1 created").arg(folderName));
            else
                m_statusbar->Message(QString("Error creating %1").arg(folderName),
                                     MessageType::ERROR, 5);
        }

        m_highlight_text = folderNames.last();
    }
}

void FilePanel::NewFile(const QStringList &fileNames) noexcept {
    QFile file;
    if (fileNames.isEmpty()) {
        bool ok;
        QString fileName = utils::getInput(this, "Create new file", "Enter filename", ok);

        if (!ok)
            return;

        if (fileName.isEmpty() || fileName.isNull()) {
            m_statusbar->Message(QString("Filename cannot be empty"),
                                 MessageType::WARNING);
            return;
        }

        file.setFileName(m_current_dir + QDir::separator() + fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            m_statusbar->Message(QString("File %1 created").arg(fileName));
            m_highlight_text = fileName;
        } else
        m_statusbar->Message(QString("Error creating %1").arg(fileName),
                             MessageType::ERROR, 5);
    } else {
        for (const auto &fileName : fileNames) {
            file.setFileName(m_current_dir + QDir::separator() + fileName);
            if (file.open(QIODevice::WriteOnly)) {
                file.close();
                m_statusbar->Message(QString("File %1 created").arg(fileName));
            } else {
                m_statusbar->Message(QString("Error creating %1").arg(fileName),
                                     MessageType::ERROR, 5);
            }
        }
        m_highlight_text = fileNames.last();
    }
}

void FilePanel::wheelEvent(QWheelEvent *e) {
    if (m_scroll_action) {
        auto angleDeltaY = e->angleDelta().y();
        if (angleDeltaY > 0)
            PrevItem();
        else if (angleDeltaY < 0)
            NextItem();
    }
}

void FilePanel::ToggleMouseScroll() noexcept {
    m_scroll_action = !m_scroll_action;
}

void FilePanel::ToggleMouseScroll(const bool &state) noexcept {
    m_scroll_action = state;
}

void FilePanel::dragRequested() noexcept {
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    auto rows = m_selection_model->selectedRows(m_file_name_column_index);
    auto filePathList = m_model->getFilePathsFromIndexList(rows);
    QList<QUrl> urls;
    urls.reserve(filePathList.size());

    for (const auto &filePath : filePathList)
    urls.append(QUrl::fromLocalFile(filePath));

    mimeData->setUrls(urls);
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction;
    switch (m_file_op_type) {

        case FileWorker::FileOPType::COPY:
            dropAction = drag->exec(Qt::CopyAction);
            break;

        case FileWorker::FileOPType::CUT:
            dropAction = drag->exec(Qt::MoveAction);
            break;
    }
}


void FilePanel::ToggleVisualLine() noexcept {
    m_visual_line_mode = !m_visual_line_mode;
    if (m_visual_line_mode) {
        m_visual_start_index = m_table_view->currentIndex();
        m_selection_model->select(m_visual_start_index,
                                  QItemSelectionModel::Rows);
        m_table_view->setSelectionMode(QAbstractItemView::MultiSelection);
        m_hook_manager->triggerHook("visual_line_mode_on");
    } else {
        m_table_view->clearSelection();
        m_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_selection_model->setCurrentIndex(m_selection_model->currentIndex(),
                                           QItemSelectionModel::Select | QItemSelectionModel::Rows);
        m_hook_manager->triggerHook("visual_line_mode_off");
    }

    m_statusbar->SetVisualLineMode(m_visual_line_mode);
}

void FilePanel::ToggleVisualLine(const bool &state) noexcept {
    m_visual_line_mode = state;
    if (state) {
        m_visual_start_index = m_table_view->currentIndex();
        m_selection_model->select(m_visual_start_index,
                                  QItemSelectionModel::Rows);
        m_table_view->setSelectionMode(QAbstractItemView::MultiSelection);
    } else {
        m_table_view->clearSelection();
        m_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_selection_model->setCurrentIndex(m_selection_model->currentIndex(),
                                           QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    m_statusbar->SetVisualLineMode(state);
}

void FilePanel::AsyncShellCommand(const QString &command) noexcept {
    UNUSED(command);
}

void FilePanel::ShellCommand(const QString &command) noexcept {
    UNUSED(command);
}

const FilePanel::ItemProperty FilePanel::getItemProperty() noexcept {
    ItemProperty property;
    QLocale locale;
    QMimeDatabase db;
    auto file = getCurrentItem();
    auto fileInfo = QFileInfo(file);
    property.name = fileInfo.fileName().toStdString();
    property.size = locale.formattedDataSize(fileInfo.size()).toStdString();
    property.mimeName = db.mimeTypeForFile(file).name().toStdString();

    return property;
}

void FilePanel::HomeDirectory() noexcept {
    setCurrentDir("~");
}

void FilePanel::PreviousDirectory() noexcept {
    setCurrentDir(m_previous_dir_path);
}

void FilePanel::SelectFirstItem() noexcept {
    if (m_model->rowCount() > 0) {
        m_highlight_index = m_model->index(0, 0);
        m_table_view->setCurrentIndex(m_model->index(0, 0));
    }
}

void FilePanel::goto_symlink_target() noexcept {
    auto file_name = m_model->data(m_table_view->currentIndex()).toString();
    auto separator = m_model->getSymlinkSeparator();
    if (file_name.contains(separator)) {
        auto target_path = file_name.split(separator).at(1).trimmed();
        setCurrentDir(target_path, true);
    } else {
        m_statusbar->Message("Item is not a symlink", MessageType::WARNING);
    }
}


void FilePanel::copy_to() noexcept {
    QFileDialog fd;
    fd.setFileMode(QFileDialog::FileMode::Directory);
    fd.setOptions(QFileDialog::Option::ShowDirsOnly);
    if (fd.exec()) {
        QString path = fd.selectedFiles().last();
        if (!(path.isEmpty() || path.isNull())) {
            CopyDWIM();
            PasteItems(path);
        }
    }
}

void FilePanel::move_to() noexcept {
    QFileDialog fd;
    fd.setFileMode(QFileDialog::FileMode::Directory);
    fd.setOptions(QFileDialog::Option::ShowDirsOnly);
    if (fd.exec()) {
        QString path = fd.selectedFiles().last();
        CutDWIM();
        PasteItems(path);
    }

}

void FilePanel::link_to() noexcept {
    QFileDialog fd;
    fd.setFileMode(QFileDialog::FileMode::Directory);
    fd.setOptions(QFileDialog::Option::ShowDirsOnly);
    if (fd.exec()) {
        QString path = fd.selectedFiles().last();
        QStringList files;
        if (m_model->getMarkedFilesCountLocal() != 0)
            files = m_model->getMarkedFilesLocal();
        else
            files << getCurrentItem();

        LinkItems(files, path);
    }
}

void FilePanel::LinkItems(const QStringList &files, const QString &target_path,
                          const bool &hard_link) noexcept {

    if (hard_link) {
        // TODO: Hard Link
    } else {
        // Soft links
        for (const auto &path : files) {
            auto fileName = QFileInfo(path).fileName();
            auto newFileName = joinPaths(target_path, fileName);
            if (!QFile::link(path, newFileName)) {
                m_statusbar->Message(QString("Could not link file %1").arg(path),
                                     MessageType::ERROR);
            }
        }
    }
}

bool FilePanel::is_current_item_a_file() noexcept {
    return QFileInfo(getCurrentItem()).isFile();
}

void FilePanel::NextPage() noexcept {
    auto currentIndex = m_table_view->currentIndex();
    QModelIndex index = m_table_view->model()->index(currentIndex.row() + 10, 0);
    if (!index.isValid()) {
        return;
    }
    m_table_view->scrollTo(index, QAbstractItemView::PositionAtCenter);
    m_table_view->setCurrentIndex(index);

    if (m_visual_line_mode) {
        // Update selection to include the new index
        QItemSelection selection(m_visual_start_index, index);
        m_selection_model->select(selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                  QItemSelectionModel::SelectionFlag::Rows);
        m_selection_model->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
        m_table_view->viewport()->update();
        /*m_table_view->viewport()->update();*/
    }
}

void FilePanel::PrevPage() noexcept {
    auto currentIndex = m_table_view->currentIndex();
    QModelIndex index = m_table_view->model()->index(currentIndex.row() - 10, 0);
    if (!index.isValid()) {
        return;
    }
    m_table_view->scrollTo(index, QAbstractItemView::PositionAtCenter);
    m_table_view->setCurrentIndex(index);

    if (m_visual_line_mode) {
        // Update selection to include the new index
        QItemSelection selection(m_visual_start_index, index);
        m_selection_model->select(selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                  QItemSelectionModel::SelectionFlag::Rows);
        m_selection_model->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
        m_table_view->viewport()->update();
        /*m_table_view->viewport()->update();*/
    }

}
