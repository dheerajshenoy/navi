#include "FilePanel.hpp"
#include "FileWorker.hpp"
#include <qnamespace.h>

FilePanel::FilePanel(Inputbar *inputBar, Statusbar *statusBar, QWidget *parent)
: m_inputbar(inputBar), m_statusbar(statusBar), QWidget(parent) {
    this->setLayout(m_layout);
    m_layout->addWidget(m_table_view);
    m_table_view->setModel(m_model);

    // m_model->setNameFilterDisables(false);

    m_terminal_args = QStringList() << "--working-directory" << "%d";

    initSignalsSlots();
    initContextMenu();

    m_file_name_column_index = m_model->fileNameColumnIndex();

    m_table_view->horizontalHeader()->setSectionResizeMode(0,
                                                           QHeaderView::Stretch);
    // m_table_view->horizontalHeader()->resizeSections(i,
    // QHeaderView::Interactive);
    setAcceptDrops(true);
    this->show();
}

void FilePanel::ResetFilter() noexcept {
    m_model->setNameFilters(QStringList() << "*");
    ForceUpdate();
}

void FilePanel::initContextMenu() noexcept {

    m_context_action_open = new QAction("Open");
    m_context_action_open_with = new QAction("Open With");
    m_context_action_open_terminal = new QAction("Open Terminal Here");
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

    connect(m_context_action_open_terminal, &QAction::triggered, this,
            [&]() { OpenTerminal(); });

    connect(m_context_action_open, &QAction::triggered, this,
            [&]() { SelectItem(); });

    connect(m_context_action_open_with, &QAction::triggered, this,
            [&]() { OpenWith(); });

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

Result<bool> FilePanel::OpenTerminal(const QString &directory) noexcept {

    if (directory.isEmpty()) {
        QStringList args = m_terminal_args;
        bool res = QProcess::startDetached(
                                           m_terminal, args.replaceInStrings("%d", m_current_dir));
        return Result(res);
    } else {
        if (QDir(directory).exists()) {
            m_terminal_args[0].replace("%d", directory);
            // m_terminal_arg.replace("%d", directory);
            bool res = QProcess::startDetached(m_terminal, m_terminal_args);
            return Result(res);
        }
        return Result(false, "Directory doesn't exist");
    }
}

void FilePanel::initSignalsSlots() noexcept {

    connect(m_table_view, &TableView::dragRequested, this,
            &FilePanel::dragRequested);

    connect(m_model, &FileSystemModel::directoryLoadProgress, this,
            [&](const int &progress) {});

    connect(this, &FilePanel::dropCopyRequested, this,
            [&](const QStringList &sourceFilePaths) {
                m_file_op_type = FileOPType::COPY;
                m_register_files_list = sourceFilePaths;
                PasteItems();
            });

    connect(this, &FilePanel::dropCutRequested, this,
            [&](const QStringList &sourceFilePaths) {
                m_file_op_type = FileOPType::CUT;
                m_register_files_list = sourceFilePaths;
                PasteItems();
            });

    connect(m_table_view, &QTableView::doubleClicked, this,
            &FilePanel::handleItemDoubleClicked);

    connect(m_table_view->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [&](const QModelIndex &current, const QModelIndex &previous) {
        QModelIndex fileNameIndex =
            current.siblingAtColumn(m_file_name_column_index);
        emit currentItemChanged(
                                m_current_dir + QDir::separator() +
                                m_model->data(fileNameIndex, Qt::DisplayRole).toString());
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
                            return;
                        }
                        m_table_view->clearSelection();
                        return;
                    }
                    m_hidden_files_just_toggled = false;
                } else {

                    if (!(m_highlight_text.isEmpty())) {
                        m_table_view->selectRow(m_model->getRowFromBaseName(m_highlight_text));
                        m_highlight_text.clear();
                        return;
                    }

                    if (m_highlight_row < rowCount) {
                        m_table_view->selectRow(m_highlight_row);
                    } else if (m_highlight_row > 0) {
                        m_table_view->selectRow(m_highlight_row - 1);
                    } else if (m_highlight_row == -1) {
                        m_table_view->selectRow(0);
                    }
                    else
                        m_table_view->clearSelection();
                }
            });

    connect(m_model->getFileSystemWatcher(),
            &QFileSystemWatcher::directoryChanged, m_model,
            [&](const QString &path) {
                m_highlight_row = m_table_view->currentIndex().row();
                m_model->loadDirectory(path);
            });
}

void FilePanel::DropCopyRequested(const QStringList &sourcePaths) noexcept {
    QString destDir = m_current_dir;

    QThread *thread = new QThread();
    FileWorker *worker = new FileWorker(sourcePaths, destDir, FileOPType::COPY);
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

    QThread *thread = new QThread();
    FileWorker *worker = new FileWorker(sourcePaths, destDir, FileOPType::CUT);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &FileWorker::performOperation);

    connect(worker, &FileWorker::finished, this,
            [&]() { emit fileOperationDone(true); });

    connect(worker, &FileWorker::error, this, [&](const QString &reason) {
        emit fileOperationDone(false, reason);
    });
    // connect(worker, &FileWorker::canceled, this,
    // &FileCopyWidget::handleCanceled);

    connect(worker, &FileWorker::finished, thread, &QThread::quit);
    connect(worker, &FileWorker::finished, worker, &FileWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

QString FilePanel::getCurrentItem() noexcept {
    return m_current_dir + QDir::separator() +
           m_model->data(m_table_view->currentIndex()).toString();
}

QString FilePanel::getCurrentItemFileName() noexcept {
    return m_model->data(m_table_view->currentIndex())
      .toString()
      .split(m_model->getSymlinkSeparator())
      .at(0);
}

void FilePanel::handleItemDoubleClicked(const QModelIndex &index) noexcept {
    selectHelper(index, true);
}

void FilePanel::setCurrentDir(QString path,
                              const bool &SelectFirstItem) noexcept {
    if (path.isEmpty())
        return;

    if (path.contains("~")) {
        path = path.replace("~", QDir::homePath());
    }

    if (utils::isValidPath(path)) {
        m_model->setRootPath(path);
        m_current_dir = path;

        if (SelectFirstItem)
            m_table_view->selectRow(0);
        m_search_new_directory = true;
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

    QModelIndex nextIndex = m_model->index(nextRow, 0);
    if (nextIndex.isValid()) {
        m_table_view->setCurrentIndex(nextIndex);
        m_table_view->scrollTo(nextIndex);

        if (m_visual_line_mode) {
            // Update selection to include the new index
            QItemSelection selection(m_visual_start_index, nextIndex);
            m_table_view->selectionModel()->select(
                                                   selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                                              QItemSelectionModel::SelectionFlag::Rows);
            m_table_view->selectionModel()->setCurrentIndex(
                                                            nextIndex, QItemSelectionModel::NoUpdate);
        }
    }

    // TODO: Add hook
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
            m_table_view->selectionModel()->select(
                                                   selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                                              QItemSelectionModel::SelectionFlag::Rows);
            m_table_view->selectionModel()->setCurrentIndex(
                                                            prevIndex, QItemSelectionModel::NoUpdate);
        }
    }
}

void FilePanel::selectHelper(const QModelIndex &index,
                             const bool selectFirst) noexcept {
    QString filepath = m_model->filePath(index);
    if (m_model->isDir(index)) {
        setCurrentDir(filepath, selectFirst);
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
    }
    // TODO: Add hook
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

void FilePanel::UpDirectory() noexcept {
    QString old_dir = m_current_dir;
    QDir currentDir(old_dir);

    if (currentDir.cdUp()) {
        if (m_visual_line_mode)
            m_visual_line_mode = false;
        setCurrentDir(currentDir.absolutePath());
        QModelIndex oldDirIndex = m_model->index(old_dir);
        m_table_view->setCurrentIndex(oldDirIndex);
        m_table_view->scrollTo(oldDirIndex);
        emit afterDirChange(m_current_dir);
    }
    // TODO: Add hook
}

void FilePanel::ToggleMarkItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    if (m_model->data(currentIndex, static_cast<int>(Role::Marked)).toBool()) {
        m_model->setData(currentIndex, false, static_cast<int>(Role::Marked));
        m_model->removeMarkedFile(currentIndex);
    } else {
        m_model->setData(currentIndex, true, static_cast<int>(Role::Marked));
    }
}

void FilePanel::ToggleMarkDWIM() noexcept {
    if (m_table_view->selectionModel()->hasSelection()) {
        auto indexes = m_table_view->selectionModel()->selectedRows();
        for (const auto &index : indexes) {
            if (m_model->data(index, static_cast<int>(Role::Marked)).toBool()) {
                m_model->setData(index, false, static_cast<int>(Role::Marked));
                m_model->removeMarkedFile(index);
            } else {
                m_model->setData(index, true, static_cast<int>(Role::Marked));
            }
        }
    } else {
        ToggleMarkItem();
    }
    ToggleVisualLine(false);
}

void FilePanel::MarkItem() noexcept {
    m_model->setData(m_table_view->currentIndex(), true,
                     static_cast<int>(Role::Marked));
}

void FilePanel::MarkItems(const QModelIndexList &list) noexcept {
    for (auto index : list)
        m_model->setData(index, true, static_cast<int>(Role::Marked));
    ToggleVisualLine(false);
}

void FilePanel::MarkDWIM() noexcept {
    if (m_table_view->selectionModel()->hasSelection()) {
        MarkItems(m_table_view->selectionModel()->selectedIndexes());
    } else {
        MarkItem();
    }
    ToggleVisualLine(false);
}

void FilePanel::UnmarkDWIM() noexcept {
    if (m_table_view->selectionModel()->hasSelection()) {
        UnmarkItems(m_table_view->selectionModel()->selectedIndexes());
    } else {
        MarkItem();
    }
    ToggleVisualLine(false);
}

void FilePanel::UnmarkItems(const QModelIndexList &list) noexcept {
    for (auto index : list) {
        m_model->setData(index, false, static_cast<int>(Role::Marked));
        m_model->removeMarkedFile(index);
    }
    ToggleVisualLine(false);
}

void FilePanel::UnmarkItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    m_model->removeMarkedFile(currentIndex);
}

void FilePanel::UnmarkItemsGlobal() noexcept {
    if (m_model->hasMarks()) {
        QString confirm =
            m_inputbar
            ->getInput(QString("Do you want to clear %1 global marks ? (y/N)")
                           .arg(m_model->getMarkedFilesCount()))
            .toLower();

        if (confirm == "y")
            m_model->removeMarkedFiles();
        else
            m_statusbar->Message("Unmark cancelled");
    } else
        m_statusbar->Message("No marks found");
}

void FilePanel::UnmarkItemsLocal() noexcept {
    if (m_model->hasMarksLocal()) {
        // auto nMarks = m_model->getMarkedFilesCountLocal();
        // QString confirm =
        //     m_inputbar
        //         ->getInput(QString("Do you want to clear %1 local marks ? (y/N)")
        //                        .arg(nMarks))
        //         .toLower();
        // if (confirm == "y")
        m_model->clearMarkedFilesListLocal();
        // else
        //   m_statusbar->Message("Unmark cancelled", MessageType::WARNING);
    } else
        m_statusbar->Message("No local marks found", MessageType::WARNING);
}

void FilePanel::GotoFirstItem() noexcept {
    QModelIndex index = m_model->index(0, 0);
    m_table_view->setCurrentIndex(index);

    if (m_visual_line_mode) {
        // Update selection to include the new index
        QItemSelection selection(m_visual_start_index, index);
        m_table_view->selectionModel()->select(
                                               selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                                          QItemSelectionModel::SelectionFlag::Rows);
        m_table_view->selectionModel()->setCurrentIndex(
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
        m_table_view->selectionModel()->select(
                                               selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                                          QItemSelectionModel::SelectionFlag::Rows);
        m_table_view->selectionModel()->setCurrentIndex(
                                                        index, QItemSelectionModel::NoUpdate);
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
            m_table_view->selectionModel()->select(
                                                   selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                                              QItemSelectionModel::SelectionFlag::Rows);
            m_table_view->selectionModel()->setCurrentIndex(
                                                            index, QItemSelectionModel::NoUpdate);
        }
    }
}

void FilePanel::GotoMiddleItem() noexcept {
    unsigned int middleRow = m_model->rowCount() / 2;

    QModelIndex index = m_model->index(middleRow, 0);
    m_table_view->setCurrentIndex(index);

    if (m_visual_line_mode) {
        // Update selection to include the new index
        QItemSelection selection(m_visual_start_index, index);
        m_table_view->selectionModel()->select(
                                               selection, QItemSelectionModel::SelectionFlag::ClearAndSelect |
                                                          QItemSelectionModel::SelectionFlag::Rows);
        m_table_view->selectionModel()->setCurrentIndex(
                                                        index, QItemSelectionModel::NoUpdate);
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
            QString inputConfirm =
                m_inputbar
              ->getInput(QString("Do you want to delete %1 and all of it's "
                                 "subitems ? (y, N, y!, n!)")
                             .arg(filePath))
              .toLower();
            if (inputConfirm == "y") {
                if (QFile::moveToTrash(filePath)) {
                    m_model->removeMarkedFile(filePath);
                    m_statusbar->Message(QString("Trashed item %1 successfully!")
                                   .arg(QFileInfo(filePath).fileName()));
                } else
                    m_statusbar->Message(QString("Error trashing item %1 successfully!")
                                   .arg(QFileInfo(filePath).fileName()));
            } else if (inputConfirm == "y!") {
                check = false;
                if (QFile::moveToTrash(filePath)) {
                    m_model->removeMarkedFile(filePath);
                    m_statusbar->Message(QString("Trashed item %1 successfully!")
                                   .arg(QFileInfo(filePath).fileName()));
                } else
                    m_statusbar->Message(QString("Error trashing item %1 successfully!")
                                   .arg(QFileInfo(filePath).fileName()));
            } else if (inputConfirm == "n!") {
                m_statusbar->Message("Trash operation cancelled");
                return;
            }
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
    ForceUpdate();
}

void FilePanel::RenameItem() noexcept {
    // current selection single rename
    const QString &oldName = getCurrentItem();
    QFileInfo fileInfo(oldName);
    const QString &oldFileName = fileInfo.fileName();
    QModelIndex currentIndex = m_table_view->currentIndex();
    QString newFileName;

    if (fileInfo.isFile())
        newFileName = m_inputbar->getInput(QString("Rename (%1)").arg(oldFileName),
                                           oldFileName, fileInfo.baseName());
    else if (fileInfo.isDir())
        newFileName = m_inputbar->getInput(QString("Rename (%1)").arg(oldFileName),
                                           oldFileName, oldFileName);

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
            QModelIndex currentIndex = m_table_view->currentIndex();
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
            QModelIndex currentIndex = m_table_view->currentIndex();
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
        QString inputConfirm =
            m_inputbar->getInput(QString("Do you want to delete %1 and all of it's subitems ? (y, N)").arg(itemPath)).toLower();
        if (inputConfirm == "y") {
            dir.removeRecursively();
            m_statusbar->Message(QString("Deleted %1 successfully").arg(itemPath));
            m_model->removeMarkedFile(itemPath);
        } else {
            m_statusbar->Message("Delete operation cancelled");
            return;
        }
    } else {
        QString inputConfirm =
            m_inputbar->getInput(QString("Do you want to delete %1 ? (y, N)").arg(itemPath))
            .toLower();
        if (inputConfirm == "y") {
            if (QFile::remove(itemPath)) {
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

    m_highlight_row = currentIndex.row();
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
        m_highlight_row = m_table_view->currentIndex().row();
        m_model->addDirFilter(QDir::Hidden);
        m_highlight_text = getCurrentItemFileName();
    }
    else {
        m_model->removeDirFilter(QDir::Hidden);
        m_highlight_text = getCurrentItemFileName();
    }
    m_model->loadDirectory(m_current_dir);
}

void FilePanel::ToggleDotDot() noexcept {
    m_dot_dot_shown = !m_dot_dot_shown;
    if (m_dot_dot_shown)
        m_model->removeDirFilter(QDir::NoDotDot);
    else
        m_model->addDirFilter(QDir::NoDotDot);

    m_highlight_row = m_table_view->currentIndex().row();
    m_model->loadDirectory(m_current_dir);
}

void FilePanel::Search(QString searchExpression) noexcept {
    m_inputbar->disableCommandCompletions();
    if (searchExpression.isNull() || searchExpression.isEmpty())
        searchExpression =
            m_inputbar->getInput("Search", m_search_text, m_search_text);
    m_search_index_list = m_model->match(m_model->index(0, 0), Qt::DisplayRole, searchExpression,
                                         -1, Qt::MatchRegularExpression);
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

    if (m_search_index_list.isEmpty())
        return;

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

    if (m_search_index_list.isEmpty())
        return;

    m_search_index_list_index--;
    if (m_search_index_list_index < 0) {
        m_search_index_list_index = m_search_index_list.size() - 1;
        m_statusbar->Message("Search reached BOTTOM of the directory");
    }

    m_table_view->setCurrentIndex(m_search_index_list.at(m_search_index_list_index));
    m_statusbar->SetSearchMatchIndex(m_search_index_list_index);
}

void FilePanel::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    if (m_model->hasMarks() || m_table_view->selectionModel()->hasSelection()) {
        menu.addAction(m_context_action_open);
        menu.addAction(m_context_action_open_terminal);
        menu.addAction(m_context_action_open_with);
        menu.addAction(m_context_action_cut);
        menu.addAction(m_context_action_copy);
        menu.addAction(m_context_action_paste);
        menu.addAction(m_context_action_delete);
        menu.addAction(m_context_action_rename);
        menu.addAction(m_context_action_trash);
        menu.addAction(m_context_action_properties);
    } else {
        menu.addAction(m_context_action_open_terminal);
        menu.addAction(m_context_action_paste);
        menu.addAction(m_context_action_properties);
    }
    menu.exec(event->globalPos());
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

void FilePanel::PasteItems() noexcept {
    if (!m_register_files_list.isEmpty()) {
        QString destDir = m_current_dir;

        const QStringList &filesList =
            QStringList(m_register_files_list.cbegin(), m_register_files_list.cend());

        QThread *thread = new QThread();
        FileWorker *worker = new FileWorker(filesList, destDir, m_file_op_type);

        connect(thread, &QThread::started, worker, &FileWorker::performOperation);

        connect(worker, &FileWorker::finished, this, [&]() {
            emit fileOperationDone(true);
            m_model->removeMarkedFiles(filesList);
        });

        connect(worker, &FileWorker::error, this, [&](const QString &reason) {
            emit fileOperationDone(false, reason);
        });

        worker->moveToThread(thread);
        // TODO: Add Progress
        // connect(worker, &FileCopyWorker::progress, this,
        // &FileCopyWidget::updateProgress);

        connect(worker, &FileWorker::finished, thread, &QThread::quit);
        connect(worker, &FileWorker::finished, worker, &FileWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }
}

void FilePanel::CopyDWIM() noexcept {

    auto localMarks = m_model->getMarkedFilesLocal();

    if (localMarks.isEmpty()) {
        CopyItem();
        return;
    }
    m_file_op_type = FileOPType::COPY;
    m_register_files_list = localMarks;
}

void FilePanel::CopyItem() noexcept {
    m_file_op_type = FileOPType::COPY;
    m_register_files_list = {getCurrentItem()};
}

void FilePanel::CopyItemsLocal() noexcept {
    m_file_op_type = FileOPType::COPY;
    m_register_files_list = m_model->getMarkedFilesLocal();
}

void FilePanel::CopyItemsGlobal() noexcept {
    m_file_op_type = FileOPType::COPY;
    m_register_files_list = m_model->getMarkedFiles();
}

void FilePanel::CutDWIM() noexcept {

    auto localMarks = m_model->getMarkedFilesLocal();

    if (localMarks.isEmpty()) {
        CutItem();
        return;
    }

    m_file_op_type = FileOPType::CUT;
    m_register_files_list = localMarks;
}

void FilePanel::CutItem() noexcept {
    m_file_op_type = FileOPType::CUT;
    m_register_files_list = {getCurrentItem()};
}

void FilePanel::CutItemsLocal() noexcept {
    m_file_op_type = FileOPType::CUT;
    m_register_files_list = m_model->getMarkedFilesLocal();
}

void FilePanel::CutItemsGlobal() noexcept {
    m_file_op_type = FileOPType::CUT;
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

    if (mimeData->hasUrls()) {
        QStringList files;
        files.reserve(mimeData->urls().size());

        for (const QUrl &url : mimeData->urls()) {
            QString sourcePath = url.toLocalFile();
            files.append(sourcePath);
        }

        // QString destPath =
        //     QFileInfo(m_model->rootPath()).absoluteFilePath();

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
    QModelIndexList selectedIndexes =
        m_table_view->selectionModel()->selectedIndexes();

    if (selectedIndexes.isEmpty())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    if (!m_model)
        return;

    // Collect file paths
    QList<QUrl> urls;
    for (const QModelIndex &index : selectedIndexes) {
        QString filePath = m_model->rootPath() + QDir::separator() +
                           m_model->data(index, Qt::DisplayRole).toString();
        urls << QUrl::fromLocalFile(filePath);
    }

    mimeData->setUrls(urls);

    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}

void FilePanel::ShowItemPropertyWidget() noexcept {
    FilePropertyWidget prop_widget(getCurrentItem());
    prop_widget.exec();
}

void FilePanel::MarkInverse() noexcept {

    if (!m_model->hasMarksLocal())
        return;

    auto nitems = m_model->rowCount();
    for (int i = 0; i < nitems; i++) {
        QModelIndex index = m_model->index(i, 0);
        bool isMarked =
            m_model->data(index, static_cast<int>(Role::Marked)).toBool();
        if (isMarked) {
            m_model->setData(index, false, static_cast<int>(Role::Marked));
            m_model->removeMarkedFile(index);
        } else
            m_model->setData(index, true, static_cast<int>(Role::Marked));
    }
}

void FilePanel::MarkAllItems() noexcept {
    auto nitems = m_model->rowCount();
    for (int i = 0; i < nitems; i++) {
        QModelIndex index = m_model->index(i, 0);
        m_model->setData(index, true, static_cast<int>(Role::Marked));
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
        QString fileName = m_inputbar->getInput("Enter filename");
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

    auto indexes = m_table_view->selectionModel()->selectedIndexes();
    auto filePathList = m_model->getFilePathsFromIndexList(indexes);
    QList<QUrl> urls;
    urls.reserve(filePathList.size());

    for (const auto &filePath : filePathList)
        urls.append(QUrl::fromLocalFile(filePath));

    mimeData->setUrls(urls);
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction;
    switch (m_file_op_type) {

    case FileOPType::COPY:
        dropAction = drag->exec(Qt::CopyAction);
        break;

    case FileOPType::CUT:
        dropAction = drag->exec(Qt::MoveAction);
        break;
    }
}

void FilePanel::ToggleHeaders(const bool &state) noexcept {
    m_table_view->horizontalHeader()->setVisible(state);
}

void FilePanel::ToggleHeaders() noexcept {
    m_table_view->horizontalHeader()->setVisible(
                                                 !m_table_view->horizontalHeader()->isVisible());
}

void FilePanel::SetCycle(const bool &state) noexcept { m_cycle_item = state; }

void FilePanel::ToggleCycle() noexcept { m_cycle_item = !m_cycle_item; }

void FilePanel::ToggleVisualLine() noexcept {
    m_visual_line_mode = !m_visual_line_mode;
    if (m_visual_line_mode) {
        m_visual_start_index = m_table_view->currentIndex();
        m_table_view->selectionModel()->select(m_visual_start_index,
                                               QItemSelectionModel::Rows);
        m_table_view->setSelectionMode(QAbstractItemView::MultiSelection);
    } else {
        m_table_view->clearSelection();
        m_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_table_view->selectionModel()->setCurrentIndex(
                                                        m_table_view->selectionModel()->currentIndex(),
                                                        QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    m_statusbar->SetVisualLineMode(m_visual_line_mode);
}

void FilePanel::ToggleVisualLine(const bool &state) noexcept {
    m_visual_line_mode = state;
    if (state) {
        m_visual_start_index = m_table_view->currentIndex();
        m_table_view->selectionModel()->select(m_visual_start_index,
                                               QItemSelectionModel::Rows);
        m_table_view->setSelectionMode(QAbstractItemView::MultiSelection);
    } else {
        m_table_view->clearSelection();
        m_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_table_view->selectionModel()->setCurrentIndex(
                                                        m_table_view->selectionModel()->currentIndex(),
                                                        QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    m_statusbar->SetVisualLineMode(state);
}

void FilePanel::AsyncShellCommand(const QString &command) noexcept {}

void FilePanel::ShellCommand(const QString &command) noexcept {}

void FilePanel::OpenWith() noexcept {}

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
