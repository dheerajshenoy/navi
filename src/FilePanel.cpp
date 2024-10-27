#include "FilePanel.hpp"

FilePanel::FilePanel(QWidget *parent) : QWidget(parent) {
    this->setLayout(m_layout);
    m_layout->addWidget(m_table_view);
    m_table_view->setModel(m_model);
    this->show();

    // m_model->setNameFilterDisables(false);

    if (m_terminal.isEmpty() || m_terminal.isNull())
        m_terminal = "/usr/bin/kitty";

    m_terminal_args = QStringList() << "%d";

    initSignalsSlots();
    initKeybinds();
    initContextMenu();

    // Select the first item after the directory has loaded for the first time
    // after the application starts.
    // We disconnect the signal because we do not want to select the first item
    // when we come up the directory and so we handle it manually without the
    // signal
    connect(m_model, &FileSystemModel::directoryLoaded, this, [&]() {
      selectFirstItem();
      disconnect(m_model, &FileSystemModel::directoryLoaded, 0, 0);
    });

    connect(m_model, &FileSystemModel::directoryLoadProgress, this,
            [&](const int &progress) {});
}

void FilePanel::ResetFilter() noexcept {
    m_model->setNameFilters(QStringList() << "*");
    ForceUpdate();
    selectFirstItem();
}

void FilePanel::initContextMenu() noexcept {

    m_context_action_open = new QAction("Open");
    m_context_action_open_with = new QAction("Open With");
    m_context_action_open_terminal = new QAction("Open Terminal Here");
    m_context_action_cut = new QAction("Cut");
    m_context_action_copy = new QAction("Copy");
    m_context_action_paste = new QAction("Paste");
    m_context_action_delete = new QAction("Delete");
    m_context_action_trash = new QAction("Trash");
    m_context_action_properties = new QAction("Properties");

    connect(m_context_action_open_terminal, &QAction::triggered, this,
            [&]() { OpenTerminal(); });
}

Result<bool> FilePanel::OpenTerminal(const QString &directory) noexcept {

    if (directory.isEmpty()) {
        // m_terminal_arg.replace("%d", directory);
        m_terminal_args[0].replace("%d", m_current_dir);
        qDebug() << m_terminal_args;
        bool res = QProcess::startDetached(m_terminal, m_terminal_args);
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

void FilePanel::initKeybinds() noexcept {}

void FilePanel::initSignalsSlots() noexcept {
    connect(m_table_view, &QTableView::doubleClicked, this,
            &FilePanel::handleItemDoubleClicked);

    connect(m_table_view->selectionModel(), &QItemSelectionModel::currentChanged,
            this,
            [this](const QModelIndex &current, const QModelIndex &previous) {
                emit currentItemChanged(
                                        m_current_dir + QDir::separator() +
                                        m_model->data(current, Qt::DisplayRole).toString());
            });

    connect(m_table_view, &TableView::dropCopyRequested, this,
            &FilePanel::DropCopyRequested);

    connect(m_table_view, &TableView::dropCutRequested, this,
            &FilePanel::DropCutRequested);

    // connect(m_model, &FileSystemModel::directoryLoaded, this, [&]() {
    // });
}

void FilePanel::DropCopyRequested(const QString &sourcePath) noexcept {
    QString destDir = m_current_dir;

    const QStringList filesList = QStringList() << sourcePath;

    QThread *thread = new QThread();
    FileWorker *worker = new FileWorker(filesList, destDir, FileOPType::COPY);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker,
            &FileWorker::copyFilesWithProgress);

    connect(worker, &FileWorker::finished, this,
            [&]() { emit copyPastaDone(true); });

    connect(worker, &FileWorker::error, this,
            [&](const QString &reason) { emit copyPastaDone(false, reason); });
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

void FilePanel::DropCutRequested(const QString &sourcePath) noexcept {

    QString destDir = m_current_dir;

    const QStringList &filesList = QStringList() << sourcePath;

    QThread *thread = new QThread();
    FileWorker *worker = new FileWorker(filesList, destDir, FileOPType::CUT);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &FileWorker::cutFilesWithProgress);

    connect(worker, &FileWorker::finished, this,
            [&]() { emit cutPastaDone(true); });

    connect(worker, &FileWorker::error, this,
            [&](const QString &reason) { emit cutPastaDone(false, reason); });
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

void FilePanel::selectFirstItem() noexcept {
    // QTimer::singleShot(0, [&]() {
    QModelIndex rootIndex = m_table_view->rootIndex();
    m_item_count = m_model->rowCount(rootIndex);

    // Select the first item if available
    if (m_item_count > 0) {
        QModelIndex firstItemIndex = m_model->index(0, 0);
        if (firstItemIndex.isValid()) {
            m_table_view->setCurrentIndex(firstItemIndex);
            m_table_view->scrollTo(firstItemIndex);
            m_table_view->selectionModel()->select(firstItemIndex,
                                                   QItemSelectionModel::Select);
        }
    }
    // Emit signal for number of items in this directory
    emit dirItemCount(m_item_count);
    // });
}

void FilePanel::handleItemDoubleClicked(const QModelIndex &index) noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    QString filepath = m_model->filePath(currentIndex);
    if (m_model->isDir(index)) {
        setCurrentDir(filepath);
        selectFirstItem();
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
    }
}

bool FilePanel::isValidPath(QString path) noexcept {
    QFileInfo file(path);
    return file.exists() && file.isReadable();
}

void FilePanel::setCurrentDir(QString path) noexcept {
    if (path.isEmpty())
        return;

    if (path.contains("~")) {
        path = path.replace("~", QDir::homePath());
    }

    if (isValidPath(path)) {
        m_model->setRootPath(path);
        m_table_view->setRootIndex(m_model->index(path));
        m_current_dir = path;
        emit afterDirChange(m_current_dir);
        selectFirstItem();
    }
}

QString FilePanel::getCurrentDir() noexcept { return m_current_dir; }

FilePanel::~FilePanel() {}

///////////////////////////
// Interactive Functions //
///////////////////////////

void FilePanel::NextItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    int nextRow = currentIndex.row() + 1;

    // Loop to start
    if (nextRow >= m_model->rowCount(m_table_view->rootIndex()))
        nextRow = 0;

    QModelIndex nextIndex = m_model->index(nextRow, 0);
    if (nextIndex.isValid()) {
        m_table_view->setCurrentIndex(nextIndex);
        m_table_view->scrollTo(nextIndex);
    }
    // TODO: Add hook
}

void FilePanel::PrevItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    int prevRow = currentIndex.row() - 1;

    // // Loop
    if (prevRow < 0)
        prevRow = m_model->rowCount(m_table_view->rootIndex()) - 1;

    QModelIndex prevIndex = m_model->index(prevRow, 0);

    if (prevIndex.isValid()) {
        m_table_view->setCurrentIndex(prevIndex);
        m_table_view->scrollTo(prevIndex);
    }
    // TODO: Add hook
}

void FilePanel::SelectItem() noexcept {
    QModelIndex currentIndex = m_table_view->currentIndex();
    QString filepath = m_model->filePath(currentIndex);
    if (m_model->isDir(currentIndex)) {
        setCurrentDir(filepath);

        // Ufff.. we have to add a buffer time of 100ms or so to select the first
        // item after the directory has finished loading.
        selectFirstItem();

    } else {
        // TODO: handle File
        QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
    }
    // TODO: Add hook
}

void FilePanel::UpDirectory() noexcept {
    QString old_dir = m_current_dir;
    QDir currentDir(old_dir);

    if (currentDir.cdUp()) {
        m_current_dir = currentDir.absolutePath();
        // m_table_view->setRootIndex(m_model->index(m_current_dir));
        m_model->setRootPath(m_current_dir);
        m_table_view->setCurrentIndex(m_model->index(old_dir));
        emit afterDirChange(m_current_dir);
    }
    // TODO: Add hook
}

void FilePanel::MarkOrUnmarkItems() noexcept {
    auto indexes = m_table_view->selectionModel()->selectedIndexes();

    if (!indexes.isEmpty()) {
        for (auto index : indexes) {
            if (m_model->data(index, static_cast<int>(Role::Marked)).toBool())
                {
                    m_model->setData(index, false, static_cast<int>(Role::Marked));
                    m_model->removeMarkedFile(index);
                } else
                m_model->setData(index, true, static_cast<int>(Role::Marked));
        }
    } else {
        QModelIndex currentIndex = m_table_view->currentIndex();
        if (m_model->data(currentIndex,
                          static_cast<int>(Role::Marked)).toBool()) {
            m_model->setData(currentIndex, false,
                             static_cast<int>(Role::Marked));
            m_model->removeMarkedFile(currentIndex);
        } else
            m_model->setData(currentIndex, true,
                             static_cast<int>(Role::Marked));
    }
}

void FilePanel::MarkItems() noexcept {
    if (m_table_view->selectionModel()->hasSelection()) {
        auto indexes = m_table_view->selectionModel()->selectedIndexes();
        for (auto index : indexes) {
            m_model->setData(index, true, static_cast<int>(Role::Marked));
        }
    } else {
        m_model->setData(m_table_view->currentIndex(), true,
                         static_cast<int>(Role::Marked));
    }
}

void FilePanel::UnmarkItems() noexcept {

    if (m_table_view->selectionModel()->hasSelection()) {
        auto indexes = m_table_view->selectionModel()->selectedIndexes();
        for (auto index : indexes) {
            m_model->setData(index, false, static_cast<int>(Role::Marked));
            m_model->removeMarkedFile(index);
        }
    } else {
        QModelIndex currentIndex = m_table_view->currentIndex();
        m_model->setData(currentIndex, false, static_cast<int>(Role::Marked));
        m_model->removeMarkedFile(currentIndex);
    }
}

void FilePanel::GotoFirstItem() noexcept {
    m_table_view->setCurrentIndex(m_model->index(0, 0));
}

void FilePanel::GotoLastItem() noexcept {
    m_table_view->setCurrentIndex(m_model->index(m_model->rowCount(m_table_view->rootIndex()) - 1, 0));
}

void FilePanel::GotoItem(const uint &itemNum) noexcept {
    if (itemNum >=0 && itemNum <= m_model->rowCount(m_table_view->rootIndex()) - 1)
        m_table_view->setCurrentIndex(m_model->index(itemNum, 0));
}

bool FilePanel::TrashItems() noexcept {
    // TODO: trash items
    return true;
}

void FilePanel::Filters(const QString &filterString) noexcept {
    QStringList filterStringList = filterString.split(" ");
    if (filterStringList.isEmpty())
        filterStringList = {filterString};
    m_model->setNameFilters(filterStringList);
    ForceUpdate();
    selectFirstItem();
}

Result<bool> FilePanel::RenameItems() noexcept {
    if (m_model->hasMarks()) {
        // TODO: handle multiple rename
        return Result(true);
    } else {
        // current selection single rename
        QModelIndex currentIndex = m_table_view->currentIndex();
        QString newFileName =
            QInputDialog::getText(this, "Rename", "Enter a new name for the file");

        // If user cancels or the new file name is empty
        if (newFileName.isEmpty() || newFileName.isNull())
            return Result(false, "Operation Cancelled");

        return QFile::rename(getCurrentItem(),
                             m_current_dir + QDir::separator() + newFileName);
    }
}

bool FilePanel::DeleteItems() noexcept {
    if (m_model->hasMarks()) {
        return true;
    } else {
        if (m_model->isDir(m_table_view->currentIndex())) {
            QDir dir(getCurrentItem());
            int confirm = QMessageBox::question(
                                                this, "Delete", "Do you really want to delete ?",
                                                QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                                                QMessageBox::StandardButton::No);

            switch (confirm) {
            case QMessageBox::Yes:
                return dir.removeRecursively();
                break;

            case QMessageBox::No | QMessageBox::Cancel:
                return false;
                break;
            }
        } else {
            int confirm = QMessageBox::question(
                                                this, "Delete", "Do you really want to delete ?",
                                                QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                                                QMessageBox::StandardButton::No);
            switch (confirm) {
            case QMessageBox::Yes:
                return QFile::remove(getCurrentItem());
                break;

            case QMessageBox::No | QMessageBox::Cancel:
                return false;
                break;
            }
        }
        return false;
    }
    ForceUpdate();
}

void FilePanel::ToggleHiddenFiles() noexcept {
    m_hidden_files_shown = !m_hidden_files_shown;
    if (m_hidden_files_shown)
        m_model->setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot |
                           QDir::Hidden);
    else
        m_model->setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    ForceUpdate();
}

void FilePanel::Search(const QString &searchExpression) noexcept {
    m_search_index_list = m_model->match(m_model->index(0, 0),
                                         Qt::DisplayRole, searchExpression, -1,
                                         Qt::MatchRegularExpression);
    if (m_search_index_list.isEmpty())
        return;
    m_table_view->setCurrentIndex(m_search_index_list.at(0));
    m_search_index_list_index = 0;
}

void FilePanel::SearchNext() noexcept {
    if (m_search_index_list.isEmpty())
        return;

    m_search_index_list_index++;
    if (m_search_index_list_index > m_search_index_list.size() - 1)
        m_search_index_list_index = 0;

    m_table_view->setCurrentIndex(
                                  m_search_index_list.at(m_search_index_list_index));
}

void FilePanel::SearchPrev() noexcept {
    if (m_search_index_list.isEmpty())
        return;

    m_search_index_list_index--;
    if (m_search_index_list_index < 0)
        m_search_index_list_index = m_search_index_list.size() - 1;
    m_table_view->setCurrentIndex(
                                  m_search_index_list.at(m_search_index_list_index));
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
        menu.addAction(m_context_action_trash);
        menu.addAction(m_context_action_properties);
    } else {
        menu.addAction(m_context_action_open_terminal);
        menu.addAction(m_context_action_paste);
        menu.addAction(m_context_action_properties);
    }
    menu.exec(event->globalPos());
}

void FilePanel::ForceUpdate() noexcept { setCurrentDir(m_current_dir); }

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

bool FilePanel::Chmod(const QString &permString) noexcept {
    if (m_model->hasMarks()) {
        // auto indexes = m_table_view->selectionModel()->selectedIndexes();
        // for (auto index : indexes) {
        // }
    } else {
        QString filePath = getCurrentItem();
        return SetPermissions(filePath, permString);
    }

    return false;
}

void FilePanel::PasteItems() noexcept {
    if (m_register_for_files.size() > 0) {
        QString destDir = m_current_dir;

        const QStringList &filesList =
            QStringList(m_register_for_files.begin(), m_register_for_files.end());

        QThread *thread = new QThread();
        FileWorker *worker = new FileWorker(filesList, destDir, m_file_op_type);
        worker->moveToThread(thread);

        switch (m_file_op_type) {

        case FileOPType::COPY: {
            connect(thread, &QThread::started, worker,
                    &FileWorker::copyFilesWithProgress);

            connect(worker, &FileWorker::finished, this,
                    [&]() { emit copyPastaDone(true); });

            connect(worker, &FileWorker::error, this, [&](const QString &reason) {
                emit copyPastaDone(false, reason);
            });
        }
            // connect(worker, &FileWorker::canceled, this,
            // &FileCopyWidget::handleCanceled);
            break;

        case FileOPType::CUT: {
            connect(thread, &QThread::started, worker,
                    &FileWorker::cutFilesWithProgress);
            connect(worker, &FileWorker::finished, this,
                    [&]() { emit cutPastaDone(true); });

            connect(worker, &FileWorker::error, this,
                    [&](const QString &reason) { emit cutPastaDone(false, reason); });
            // connect(worker, &FileWorker::canceled, this,
            // &FileCopyWidget::handleCanceled);
        } break;
        }

        // TODO: Add Progress
        // connect(worker, &FileCopyWorker::progress, this,
        // &FileCopyWidget::updateProgress);

        connect(worker, &FileWorker::finished, thread, &QThread::quit);
        connect(worker, &FileWorker::finished, worker, &FileWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }
}

void FilePanel::CopyItems() noexcept {
    if (m_model->hasMarks()) {
        m_register_for_files = m_model->getMarkedFiles();
    }
    m_file_op_type = FileOPType::COPY;
}

void FilePanel::CutItems() noexcept {
    if (m_model->hasMarks()) {
        m_register_for_files = m_model->getMarkedFiles();
    }
    m_file_op_type = FileOPType::CUT;
}
