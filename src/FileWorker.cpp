#include "FileWorker.hpp"

FileWorker::FileWorker(const QStringList &files, const QString &destDir,
                       const FileOPType &type, TaskManager *taskManager,
                       Statusbar *sb, Inputbar *ib) : m_srcFiles(files), m_destDir(destDir),
m_type(type), m_task_manager(taskManager), m_statusbar(sb), m_inputbar(ib) {}

void FileWorker::copyFile(const QString &src, QString &dest) noexcept {
    QString fileName = QFileInfo(src).fileName();
    std::ifstream srcFile(src.toStdString(), std::ios::binary);

    if (!srcFile.is_open()) {
        emit error("Unable to open source file!");
        return;
    }

    assert(m_task_manager != nullptr && "Task Manager does not exist");

    dest = dest + QDir::separator() + fileName;
    std::string dest_std_str = dest.toStdString();

    if (std::filesystem::exists(dest_std_str)) {
      QString confirm = m_inputbar->getInput(QString("%1 file already exists, overwrite ? (y/N)").arg(dest)).toLower();
      if (confirm != "y" || confirm != "yes")
          return;
    }

    std::ofstream destFile(dest_std_str, std::ios::binary);
    if (!destFile.is_open()) {
        emit error("Unable to open destination file!");
        return;
    }

    Task *task = new Task();
    task->setTaskType(Task::TaskType::COPY);
    m_task_manager->addTask(task);

    const size_t bufferSize = 1024 * 1024; // 1 MB
    std::vector<char> buffer(bufferSize);

    const auto total_size = std::filesystem::file_size(src.toStdString());
    size_t bytes_copied = 0;

    while (srcFile.read(buffer.data(), buffer.size()) || srcFile.gcount() > 0) {
        destFile.write(buffer.data(), srcFile.gcount());
        bytes_copied += srcFile.gcount();
        auto _progress = static_cast<float>(bytes_copied) / total_size * 100;
        // emit progress(src, dest,
        //               static_cast<float>(bytes_copied) / total_size * 100);
        task->setProgress(_progress);
    }

    // Check for errors
    if (!srcFile.eof() || srcFile.bad()) {
        emit error(QString("Error: Failed during file copy from %1 to %2")
                        .arg(src)
                        .arg(dest));
    }

    task->setFinished(true);
    emit finished();
}

void FileWorker::cutFile(const QString &src, QString &dest) noexcept {
    QString fileName = QFileInfo(src).fileName();
    std::ifstream srcFile(src.toStdString(), std::ios::binary);

    if (!srcFile.is_open()) {
        emit error("Unable to open source file!");
        return;
    }

    assert(m_task_manager != nullptr && "Task Manager does not exist");

    dest = dest + QDir::separator() + fileName;
    std::ofstream destFile(dest.toStdString(), std::ios::binary);

    if (!destFile.is_open()) {
        emit error("Unable to open destination file!");
        return;
    }

    Task *task = new Task();
    task->setTaskType(Task::TaskType::COPY);

    const size_t bufferSize = 1024 * 1024; // 1 MB
    std::vector<char> buffer(bufferSize);

    const auto total_size = std::filesystem::file_size(src.toStdString());
    size_t bytes_copied = 0;

    while (srcFile.read(buffer.data(), buffer.size()) || srcFile.gcount() > 0) {
        destFile.write(buffer.data(), srcFile.gcount());
        bytes_copied += srcFile.gcount();
        emit progress(src, dest,
                      static_cast<float>(bytes_copied) / total_size * 100);
    }

    // Check for errors
    if (!srcFile.eof() || srcFile.bad()) {
        emit error(QString("Error: Failed during moving file from %1 to %2")
                     .arg(src)
                     .arg(dest));
        return;
    }

    if (!std::filesystem::remove(src.toStdString())) {
        emit error("Unable to delete source file");
        emit finished();
        return;
    }
    emit finished();
}

void FileWorker::copyFiles() noexcept {
    for (const auto &file : m_srcFiles) {
        copyFile(file, m_destDir);
    }
}

void FileWorker::cutFiles() noexcept {
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