#pragma once

#include "pch_fileworker.hpp"

#include "Result.hpp"
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
    void offloadOperation(const QString &src,
                          const QString &dest) noexcept;

    void decrement_and_check_operations() noexcept;

    QStringList m_srcFiles;
    QString m_destDir;   // Destination directory
    FileOPType m_type;   // The type of file operation (CUT, COPY)
    TaskManager *m_task_manager = nullptr;
    Statusbar *m_statusbar = nullptr;
    Inputbar *m_inputbar = nullptr;
    QAtomicInt m_activeOperations;
    bool m_overwrite_confirm_all = false;
};
