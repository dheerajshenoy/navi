#include "FileWorker.hpp"

FileWorker::FileWorker(const QStringList &files, const QString &destDir,
                       const FileOPType &type)
: m_srcFiles(files), m_destDir(destDir), m_type(type) {}

void FileWorker::copyFile(const QString &src, const QString &dest) noexcept {
    std::ifstream srcFile(src.toStdString(), std::ios::binary);

    if (!srcFile.is_open()) {
        emit error("Unable to open source file!");
    }

    std::ofstream destFile(dest.toStdString(), std::ios::binary);

    if (!destFile.is_open()) {
        emit error("Unable to open destination file!");
    }

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
        emit error(QString("Error: Failed during file copy from %1 to %2")
                        .arg(src)
                        .arg(dest));
    }

    emit finished();
}

void FileWorker::cutFile(const QString &src, const QString &dest) noexcept {
    std::ifstream srcFile(src.toStdString(), std::ios::binary);

    if (!srcFile.is_open()) {
        emit error("Unable to open source file!");
        return;
    }

    std::ofstream destFile(dest.toStdString(), std::ios::binary);

    if (!destFile.is_open()) {
        emit error("Unable to open destination file!");
        return;
    }

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