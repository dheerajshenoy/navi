#pragma once

#include <algorithm>
#include <QAbstractTableModel>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QModelIndex>
#include <QBrush>
#include <QColor>
#include <QLocale>
#include <QRegularExpression>
#include <QFont>
#include <QFileSystemWatcher>
#include <QMimeData>
#include <QUrl>
#include <QFileIconProvider>

#include "utils.hpp"

enum class SortColumn {
    Size,
    Name,
    Date,
};

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

    struct Column {
        QString name;
        ColumnType type;
    };

    int fileNameColumnIndex() const noexcept { return m_file_name_column_index; }
    void setSymlinkSeparator(const QString &separator) noexcept;
    void setSymlinkForeground(const QString &foreground) noexcept;
    void setSymlinkVisible(const bool &state) noexcept { m_show_symlink = state; }

    void setMarkForegroundColor(const QString &color) noexcept;
    void setMarkBackgroundColor(const QString &color) noexcept;

    void setMarkHeaderForegroundColor(const QString &color) noexcept;
    void setMarkHeaderBackgroundColor(const QString &color) noexcept;

    void setMarkFontFamily(const QString &family) noexcept;
    void setMarkHeaderFontFamily(const QString &family) noexcept;

    void setMarkFontItalic(const bool &state) noexcept;
    void setMarkFontBold(const bool &state) noexcept;

    void setMarkHeaderFontItalic(const bool &state) noexcept;
    void setMarkHeaderFontBold(const bool &state) noexcept;

    void addDirFilter(const QDir::Filters &) noexcept;
    void removeDirFilter(const QDir::Filters &) noexcept;

    QSet<QString> m_markedFiles;
    QFileSystemWatcher* getFileSystemWatcher() noexcept { return m_file_system_watcher; }
    QModelIndex getIndexFromString(const QString &path) const noexcept;
    QModelIndex getIndexFromBaseName(const QString &path) const noexcept;
    uint getMarkedFilesCount() noexcept;
    uint getMarkedFilesCountLocal() noexcept;
    void clearMarkedFilesList() noexcept;
    void clearMarkedFilesListLocal() noexcept;
    void setNameFilters(const QStringList &filters) noexcept;
    void setColumns(const QList<Column> &col) noexcept;
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
    inline bool hasMarks() noexcept { return m_markedFiles.size() > 0; }
    bool hasMarksLocal() noexcept;
    QStringList getMarkedFiles() noexcept;
    QStringList getMarkedFilesLocal() noexcept;
    void setFilter(const QDir::Filters &filters) noexcept;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    QList<QModelIndex> match(const QModelIndex &start, int role, const QVariant &value,
                             int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags()) const override;

    bool removeMarkedFile(const QString &path) noexcept;
    void removeMarkedFiles() noexcept;
    void removeMarkedFiles(const QStringList &paths) noexcept;
    bool removeMarkedFile(const QModelIndex &index) noexcept;
    inline QString getStringFromIndex(const QModelIndex &index) const noexcept {
        return this->data(index, Qt::DisplayRole).toString();
    }

    inline QStringList getFilePathsFromIndexList(const QModelIndexList &indexList) const noexcept {
        QStringList stringList;
        stringList.reserve(indexList.size());
        for (const auto &index : indexList)
            stringList.append(m_root_path + QDir::separator() + getStringFromIndex(index));
        return stringList;
    }

    inline QString getStringFromRow(const int &row, const int &col = 0) const noexcept {
        return index(row, col).data().toString();
    }

    inline QString getPathFromIndex(const QModelIndex &index) const noexcept {
        return rootPath() + QDir::separator() + getStringFromIndex(index);
    }

    inline QString getPathFromRow(const int &row) const noexcept {
        return rootPath() + QDir::separator() + getStringFromRow(row);
    }

    inline QString getSymlinkSeparator() const noexcept {
        return m_symlink_separator;
    }


    // void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    void setSortBy(const QDir::SortFlags &sortBy) noexcept;


    signals:
    void directoryLoaded(const int &rowCount);
    void directoryLoadProgress(const int &progress);
    void marksListChanged();
    void dropCopyRequested(const QStringList &sourceFilePath);
    void dropCutRequested(const QStringList &sourceFilePath);

private:
    int indexOfFileNameColumn() const noexcept;
    void initDefaults() noexcept;
    int findRow(const QFileInfo &fileInfo) const noexcept;

    QList<QFileInfo> m_fileInfoList;
    QString m_root_path;
    QHash<QString, int> m_path_row_hash;

    QDir::Filters m_dir_filters = QDir::Filter::NoDotAndDotDot |
                                  QDir::Filter::AllEntries;

    QDir::SortFlags m_dir_sort_flags = QDir::SortFlag::DirsFirst;
    QLocale m_locale;
    QStringList m_name_filters = {"*"};
    QFileSystemWatcher *m_file_system_watcher = nullptr;
    QFileIconProvider *m_fileIconProvider = new QFileIconProvider();

    QList<Column> m_column_list = {
        Column { "Name", ColumnType::FileName },
        Column { "Size", ColumnType::FileSize },
        Column { "DATE", ColumnType::FileModifiedDate },
        Column { "PERM", ColumnType::FilePermission },
    };

    QColor m_markForegroundColor = Qt::transparent,
           m_markBackgroundColor = Qt::transparent;

    QColor m_markHeaderForegroundColor = Qt::transparent,
           m_markHeaderBackgroundColor = Qt::transparent;

    QFont m_markHeaderFont, m_markFont;
    QString m_symlink_separator = "->";
    QColor m_symlink_foreground;
    bool m_show_symlink;
    unsigned int m_file_name_column_index = -1;

};