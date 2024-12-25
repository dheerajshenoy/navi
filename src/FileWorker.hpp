#pragma once

#include "Result.hpp"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QStringList>
#include <filesystem>
#include <fstream>
#include "Task.hpp"
#include "TaskManager.hpp"
#include "Statusbar.hpp"
#include "Inputbar.hpp"

class FileWorker : public QObject {
    Q_OBJECT

public:
    enum class FileOPType {
        COPY = 0,
        CUT,
    };

    explicit FileWorker(const QStringList &files, const QString &destDir,
                        const FileOPType &type, TaskManager *task_manager,
                        Statusbar *sb, Inputbar *ib);

public slots:
    void performOperation() {
        switch (m_type) {
        case FileOPType::COPY:
            copyFiles();
            break;

        case FileOPType::CUT:
            cutFiles();
            break;
        }
    }

    signals:
    void progress(const QString &sourceFile, const QString &destinationFile,
                  const float &progress);
    void finished();
    void error(const QString &message);

private:
    void copyFiles() noexcept;
    void cutFiles() noexcept;
    void copyFile(const QString &, QString) noexcept;
    void cutFile(const QString &, QString) noexcept;
    void renameFile(const QString &, const QString &) noexcept;
    void deleteFile(const QString &) noexcept;
    void trashFile(const QString &) noexcept;

    QStringList m_srcFiles;
    QString m_destDir;   // Destination directory
    FileOPType m_type;   // The type of file operation (CUT, COPY)
    TaskManager *m_task_manager = nullptr;
    Statusbar *m_statusbar = nullptr;
    Inputbar *m_inputbar = nullptr;
};
