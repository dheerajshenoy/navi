#pragma once

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>


enum class FileOPType {
    COPY = 0,
    CUT,
};

class FileWorker : public QObject {
    Q_OBJECT

    public:
    explicit FileWorker(const QStringList& files, const QString& destDir, const FileOPType& type)
        : m_files(files), m_destDir(destDir), m_type(type) {}

  public slots:
    void performOperation() {
        switch (m_type) {
        case FileOPType::COPY:
            copyFilesWithProgress();
            break;

        case FileOPType::CUT:
            cutFilesWithProgress();
            break;
      }
    }


    signals:
    void progress(const QString& sourceFile, const QString& destinationFile);
    void finished();
    void error(const QString& message);

private:
    void copyFilesWithProgress() noexcept {
        for (const QString &sourceFile : m_files) {
            QFileInfo fileInfo(sourceFile);
            QString destinationFilePath = QDir(m_destDir).filePath(fileInfo.fileName());

            // Create a QFile object to handle the copying process
            QFile source(sourceFile);
            if (!source.exists()) {
                emit error("Source file does not exist: " + sourceFile);
                continue;
            }

            if (source.copy(destinationFilePath)) {
                emit progress(sourceFile, destinationFilePath);
            } else {
                emit error("Failed to copy: " + sourceFile);
            }
        }
        emit finished();
    }

    void cutFilesWithProgress() noexcept {
        for (const QString& sourceFile : m_files) {
            QFileInfo fileInfo(sourceFile);
            QString destinationFilePath = QDir(m_destDir).filePath(fileInfo.fileName());

            // Create a QFile object to handle the copying process
            QFile source(sourceFile);
            if (!source.exists()) {
                emit error("Source file does not exist: " + sourceFile);
                continue;
            }

            if (source.copy(destinationFilePath)) {
                emit progress(sourceFile, destinationFilePath);
                source.remove();
            } else {
                emit error("Failed to cut: " + sourceFile);
            }

        }
        emit finished();
    }
    QStringList m_files;  // List of source files to copy
    QString m_destDir;    // Destination directory
    FileOPType m_type; // The type of file operation (CUT, COPY)
};
