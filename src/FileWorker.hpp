#pragma once

#include "Result.hpp"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QStringList>
#include <filesystem>
#include <fstream>

enum class FileOPType {
  COPY = 0,
  CUT,
};

class FileWorker : public QObject {
  Q_OBJECT

public:
  explicit FileWorker(const QStringList &files, const QString &destDir,
                      const FileOPType &type);

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
  void copyFile(const QString &, const QString &) noexcept;
  void cutFile(const QString &, const QString &) noexcept;

  QStringList m_srcFiles;
  QString m_destDir;   // Destination directory
  FileOPType m_type;   // The type of file operation (CUT, COPY)
};
