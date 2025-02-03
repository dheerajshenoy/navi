#include "FileWorker.hpp"
#include <qmessagebox.h>

FileWorker::FileWorker(const QStringList &files, const QString &destDir,
                       const FileOPType &type, TaskManager *taskManager) : m_srcFiles(files), m_destDir(destDir),
m_type(type), m_task_manager(taskManager) {

    connect(this, &FileWorker::finished, this, [&]() {
        m_overwrite_confirm_all = false;
    });
}

void FileWorker::copyFile(const QString &src, QString dest) noexcept {
    QString fileName = QFileInfo(src).fileName();

    assert(m_task_manager != nullptr && "Task Manager does not exist");

    dest = dest + QDir::separator() + fileName;

    if (!m_overwrite_confirm_all) {
        if (QFile::exists(dest)) {

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setWindowTitle("Overwrite File?");
            msgBox.setText(QString("The file '%1' already exists. Do you want to overwrite it?").arg(fileName));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            QPushButton *yesToAllButton = msgBox.addButton("Overwrite all files", QMessageBox::AcceptRole);
            msgBox.setDefaultButton(QMessageBox::No);

            // Show the message box and handle the response
            msgBox.exec();

            if (msgBox.clickedButton() == yesToAllButton) {
                m_overwrite_confirm_all = true;
            }
            else if (msgBox.result() != QMessageBox::No) {
                return;
            }
        }
    }

    QFuture<void> future = QtConcurrent::run([this, &src, &dest]() {
        offloadOperation(std::ref(src), std::ref(dest));
        decrement_and_check_operations();
    });
}


void FileWorker::decrement_and_check_operations() noexcept {
    if (m_activeOperations.fetchAndSubRelaxed(1) == 1) {
        emit finished();
    }
}

void FileWorker::offloadOperation(const QString &src,
                                  const QString &dest) noexcept {
    Task *task = new Task();
    task->setTaskType(Task::TaskType::COPY);
    m_task_manager->addTask(task);

    QFile srcFile(src);

    if (!srcFile.open(QIODevice::ReadOnly)) {
        emit error("Unable to open source file!");
        return;
    }

    QFile destFile(dest);

    if (!destFile.open(QIODevice::WriteOnly)) {
        emit error("Unable to open destination file!");
        return;
    }

    // Read and write in chunks
    constexpr qint64 bufferSize = 4096;
    char buffer[bufferSize];

    qint64 bytesCopied = 0;
    qint64 totalSize = srcFile.size();

    while (!srcFile.atEnd()) {
        qint64 bytesRead = srcFile.read(buffer, bufferSize);

        if (bytesRead == -1) {
            emit error("Error reading from source file");
            return;
        }

        qint64 bytesWritten = destFile.write(buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            emit error("Error writing to destination file");
            return;
        }

        bytesCopied += bytesWritten;

        auto _progress = static_cast<float>(bytesCopied) / totalSize * 100;

        // emit progress(src, dest,
        //               static_cast<float>(bytes_copied) / total_size * 100);

        task->setProgress(_progress);
    }

    task->setFinished(true);
}

void FileWorker::cutFile(const QString &src, QString dest) noexcept {

    QString fileName = QFileInfo(src).fileName();

    assert(m_task_manager != nullptr && "Task Manager does not exist");

    dest = dest + QDir::separator() + fileName;

    if (!m_overwrite_confirm_all) {
        if (QFile::exists(dest)) {

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setWindowTitle("Overwrite File?");
            msgBox.setText(QString("The file '%1' already exists. Do you want to overwrite it?").arg(fileName));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            QPushButton *yesToAllButton = msgBox.addButton("Overwrite all files", QMessageBox::AcceptRole);
            msgBox.setDefaultButton(QMessageBox::No);

            // Show the message box and handle the response
            msgBox.exec();

            if (msgBox.clickedButton() == yesToAllButton) {
                m_overwrite_confirm_all = true;
            }
            else if (msgBox.result() != QMessageBox::No) {
                return;
            }
        }
    }

    QFuture<void> future = QtConcurrent::run([this, &src, &dest]() {
        offloadOperation(std::ref(src), std::ref(dest));
        decrement_and_check_operations();
    });

    if (!QFile::remove(src)) {
        emit error("Unable to delete source file");
        return;
    }
}

void FileWorker::copyFiles() noexcept {
    m_activeOperations = m_srcFiles.size();
    for (const auto &file : m_srcFiles) {
        qDebug() << "COPY";
        copyFile(file, m_destDir);
    }
}

void FileWorker::cutFiles() noexcept {
    m_activeOperations = m_srcFiles.size();
    for (const auto &file : m_srcFiles) {
        cutFile(file, m_destDir);
    }
}

void FileWorker::renameFile(const QString &, const QString &) noexcept {
    // TODO: Rename file
    assert(0 && "Rename file unimplemented");
}

void FileWorker::deleteFile(const QString &) noexcept {
    // TODO: Delete file
    assert(0 && "Rename file unimplemented");
}

void FileWorker::trashFile(const QString &) noexcept {
    // TODO: Trash file
    assert(0 && "Rename file unimplemented");
}
