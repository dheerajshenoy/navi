#pragma once

#include <QAbstractTableModel>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QModelIndex>
#include <QBrush>
#include <QColor>
#include <QLocale>
#include <QRegularExpression>
#include "utils.hpp"

enum class Role {
  Marked = Qt::UserRole + 1,
};


class FileSystemModel : public QAbstractTableModel {
    Q_OBJECT

public:
  FileSystemModel(const QString &path, QObject *parent = nullptr);
    FileSystemModel(QObject *parent = nullptr);

    enum class ColumnType {
      FileName = 0,
      FileSize,
      FileModifiedDate,
      FilePermission,
    };

    void setNameFilters(const QStringList &filters) noexcept;
    void setColumnList(const QList<FileSystemModel::ColumnType> &cols) noexcept;
    QString filePath(const QModelIndex &index) noexcept;
    bool isDir(const QModelIndex &index) noexcept;
    QModelIndex index(const QString &path) const noexcept;
    QModelIndex index(int row, int col) const noexcept;
    void setRootPath(const QString &path) noexcept;
    QString rootPath() const noexcept { return m_root_path; }
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    void loadDirectory(const QString &path) noexcept;
    bool hasMarks() { return m_markedFiles.size() > 0; }
    QSet<QString> getMarkedFiles() noexcept { return m_markedFiles; }
    void setFilter(const QDir::Filters &filters) noexcept;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    QList<QModelIndex> match(const QModelIndex &start, int role, const QVariant &value,
                             int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags()) const override;

    bool removeMarkedFile(const QString &path) noexcept;
    bool removeMarkedFile(const QModelIndex &index) noexcept;

    signals:
    void directoryLoaded(const QString &path);
    void directoryLoadProgress(const int &progress);

private:
    void initDefaults() noexcept;
    int findRow(const QFileInfo &fileInfo) const noexcept;
    QString getStringFromIndex(const QModelIndex &index) const noexcept {
        return this->data(index, Qt::DisplayRole).toString();
    }

    QString getPathFromIndex(const QModelIndex &index) const noexcept {
        return rootPath() + QDir::separator() + getStringFromIndex(index);
    }

    QList<QFileInfo> m_fileInfoList;
    QString m_root_path;
    QSet<QString> m_markedFiles;
    QHash<QString, int> m_path_row_hash;

    QDir::Filters m_dir_filters;
    QList<ColumnType> m_column_list = {ColumnType::FileName};
    QLocale m_locale;
    QStringList m_name_filters = { "*" };
};