#include "FileSystemModel.hpp"
#include <qnamespace.h>

FileSystemModel::FileSystemModel(QObject *parent)
    : QAbstractTableModel(parent) {
  initDefaults();
}

FileSystemModel::FileSystemModel(const QString &path, QObject *parent)
    : QAbstractTableModel(parent), m_root_path(path) {
    initDefaults();
}

void FileSystemModel::initDefaults() noexcept {
    m_file_system_watcher = new QFileSystemWatcher(this);
    m_dir_filters = QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries;

    connect(m_file_system_watcher, &QFileSystemWatcher::directoryChanged, this,
            [&]() {
                loadDirectory(m_root_path);
            });
}

void FileSystemModel::clearMarkedFilesList() noexcept { m_markedFiles.clear(); }

void FileSystemModel::clearMarkedFilesListHere() noexcept {
    for (const auto &file : m_markedFiles) {
        if (file.contains(m_root_path))
            m_markedFiles.remove(file);
    }
}

uint FileSystemModel::getMarkedFilesCount() noexcept {
    return m_markedFiles.size();
}

uint FileSystemModel::getMarkedFilesCountHere() noexcept {
    uint count = 0;
    for (const auto &files : m_markedFiles)
      if (files.contains(m_root_path))
          count++;
    return count;
}

void FileSystemModel::setRootPath(const QString &path) noexcept {
    m_file_system_watcher->removePath(m_root_path);
    m_root_path = path;
    loadDirectory(path);
    m_file_system_watcher->addPath(path);
    // TODO: handle bool from this statement
}

QString FileSystemModel::filePath(const QModelIndex &index) noexcept {
  if (!index.isValid())
    return QString();

  // Assuming `fileInfoList` holds QFileInfo objects or a similar structure
  return QFileInfo(m_root_path + QDir::separator() + index.data().toString())
      .absoluteFilePath();
}

bool FileSystemModel::isDir(const QModelIndex &index) noexcept {
  if (!index.isValid())
    return false;

  // Assuming `fileInfoList` holds QFileInfo objects or a similar structure
  return QFileInfo(m_root_path + QDir::separator() + index.data().toString())
      .isDir();
}

int FileSystemModel::findRow(const QFileInfo &fileInfo) const noexcept {
  return m_path_row_hash.value(fileInfo.absoluteFilePath(),
                               -1); // Returns -1 if path not found
}

QModelIndex FileSystemModel::index(const QString &path) const noexcept {
  // Find the QFileInfo for the given path
  QFileInfo fileInfo(path);
  if (!fileInfo.exists())
    return QModelIndex(); // Return an invalid index if the path doesn't exist

  // Search for the fileInfo in your data structure and get its row (pseudo-code
  // here)
  int row = findRow(fileInfo);
  if (row == -1)
    return QModelIndex(); // Return invalid index if not found

  // Assuming `fileInfoList` is organized with parent-child relationships
  return createIndex(row, 0, &fileInfo); // Adjust the row/column as needed
}

void FileSystemModel::loadDirectory(const QString &path) noexcept {
  beginResetModel();
  m_fileInfoList.clear();
  m_path_row_hash.clear();

  QDir dir(path);

  if (dir.exists()) {
    QFileInfoList allFiles = dir.entryInfoList(m_name_filters, m_dir_filters,
                                               QDir::SortFlag::DirsFirst);
    int totalEntries = allFiles.size();
    m_fileInfoList.reserve(totalEntries);
    m_path_row_hash.reserve(totalEntries);

    for (int i = 0; i < totalEntries; i++) {
      const QFileInfo &fileInfo = allFiles.at(i);
      m_fileInfoList.append(fileInfo);
      m_path_row_hash.insert(fileInfo.absoluteFilePath(), i);

      emit directoryLoadProgress(
          static_cast<int>((static_cast<float>(i + 1) / totalEntries) * 100));
    }
  }

  endResetModel();
  emit directoryLoaded(path);
}

int FileSystemModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : m_fileInfoList.size();
}

int FileSystemModel::columnCount(const QModelIndex &parent) const {
  return parent.isValid()
             ? 0
             : m_column_list.size(); // Three columns: Name, Size, Last Modified
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    // Custom background color for marked files
    switch (role) {

        case static_cast<int>(Role::Marked):
            return m_markedFiles.contains(getPathFromIndex(index));
            break;

        case Qt::ForegroundRole: {
            bool isMarked = m_markedFiles.contains(getPathFromIndex(index));
            if (isMarked) {
                return QColor("#FF5000");
            }
            return QVariant();
        } break;

        case Qt::DisplayRole: {
            const QFileInfo &fileInfo = m_fileInfoList.at(index.row());
            switch (m_column_list.at(index.column())) {
            case ColumnType::FileName: // File Name
                return fileInfo.fileName();

            case ColumnType::FileSize: // File Size
                return fileInfo.isDir()
                    ? QVariant("<DIR>")
                    : QVariant(m_locale.formattedDataSize(fileInfo.size()));

            case ColumnType::FileModifiedDate: // Last Modified Date
                return fileInfo.lastModified().toString("yyyy-MM-dd HH:mm:ss");

            case ColumnType::FilePermission:
                return utils::getPermString(fileInfo);

            default:
                return QVariant();
            }
        }
    }

    return QVariant();
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const {
    if (orientation == Qt::Vertical) {
        switch (role) {

        case static_cast<int>(Role::Marked):
            return m_markedFiles.contains(getPathFromRow(section));
            break;

        case Qt::BackgroundRole: {
          // QColor color;
          // bool isMarked = m_markedFiles.contains(getPathFromRow(section));
          // if (isMarked)
          //     return color.fromString("#FF5000");
          // return color;
          // TODO: Fix this
        } break;

        case Qt::FontRole: {
            QFont font;
            bool isMarked = m_markedFiles.contains(getPathFromRow(section));
            if (isMarked) {
                font.setBold(true);
                font.setItalic(true);
                return font;
            }
            return font;
        }

        case Qt::DisplayRole:
            return section;
            break;

        default:
            return QVariant();
            break;

        }



    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {

        switch (m_column_list.at(section)) {
        case ColumnType::FileName:
            return "Name";
        case ColumnType::FileSize:
            return "Size";
        case ColumnType::FileModifiedDate:
            return "Last Modified";
        case ColumnType::FilePermission:
            return "Permissions";
        default:
            return QVariant();
        }
    }

    return QVariant();
}

  bool FileSystemModel::setData(const QModelIndex &index, const QVariant &value,
                                int role) {
    // if (!index.isValid() || role != Qt::EditRole || index.column() != 0)
    //     return false;

    if (role == static_cast<int>(Role::Marked)) {
        m_markedFiles.insert(getPathFromIndex(index));
        emit marksListChanged();
      emit dataChanged(index, index, {role});
      return true;
    }

    return false; // Return false if renaming fails
  }

  void FileSystemModel::setFilter(const QDir::Filters &filters) noexcept {
    m_dir_filters = filters;
  }

  QModelIndex FileSystemModel::index(int row, int col) const noexcept {
    if (row < 0 || row >= m_fileInfoList.size() || col < 0 ||
        col >= columnCount()) {
      return QModelIndex(); // Return an invalid index if out of bounds
    }

    // Create the index using the row, column, and a pointer to the QFileInfo
    // object for that row
    return createIndex(row, col, &m_fileInfoList[row]);
  }

bool FileSystemModel::removeMarkedFile(const QString &path) noexcept {
    if (m_markedFiles.contains(path)) {
        emit marksListChanged();
        return m_markedFiles.remove(path);
    }
    return false;
}

void FileSystemModel::removeMarkedFiles() noexcept {
    m_markedFiles.clear();
}

  bool FileSystemModel::removeMarkedFile(const QModelIndex &index) noexcept {
    QString path = getPathFromIndex(index);
    return m_markedFiles.contains(path) && m_markedFiles.remove(path);
  }

  void FileSystemModel::setColumnList(
      const QList<FileSystemModel::ColumnType> &cols) noexcept {
    m_column_list = cols;
  }

  void FileSystemModel::setNameFilters(
      const QStringList &name_filters) noexcept {
    m_name_filters = name_filters;
  }

  QList<QModelIndex> FileSystemModel::match(const QModelIndex &start, int role,
                                            const QVariant &value, int hits,
                                            Qt::MatchFlags flags) const {

    QList<QModelIndex> matchedIndexes;

    // Convert the search value to a QRegularExpression
    QRegularExpression regex(value.toString());
    if (!regex.isValid()) {
      return matchedIndexes; // Return empty list if regex is invalid
    }

    // Define search parameters based on provided start index
    int startRow = start.isValid() ? start.row() : 0;
    int startColumn = start.isValid() ? start.column() : 0;

    int rows = rowCount();
    int columns = columnCount();

    // Loop through rows and columns to find matching items
    for (int row = startRow; row < rows; ++row) {
      for (int col = startColumn; col < columns; ++col) {
        QModelIndex _index = index(row, col);
        if (_index.isValid()) {
          QVariant _data = data(_index, role);

          // Check if data matches the regex
          if (_data.toString().contains(regex)) {
            matchedIndexes.append(_index);

            // Stop if we've reached the desired number of hits
            if (hits > 0 && matchedIndexes.size() >= hits) {
              return matchedIndexes;
            }
          }
        }
      }
    }

    return matchedIndexes;
  }


// QMimeData *FileSystemModel::mimeData(const QModelIndexList &indexes) const {
//     auto *mimeData = new QMimeData();
//     if (indexes.isEmpty())
//         return mimeData;

//     QStringList paths;
//     for (const QModelIndex &index : indexes) {
//         if (index.isValid()) {
//             paths << index.data(Qt::DisplayRole).toString();
//         }
//     }
//     mimeData->setData("application/x-file-paths", paths.join('\n').toUtf8());
//     return mimeData;
// }

// bool FileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row,
//                                    int column, const QModelIndex &parent) {
//     if (!data->hasUrls() || !parent.isValid())
//         return false;

//     QList<QUrl> urls = data->urls();
//     QStringList urls_local;
//     urls_local.reserve(urls.size());

//     for (const auto &url : urls)
//         urls_local.append(url.toLocalFile());

//     switch (action) {
//     case Qt::DropAction::CopyAction:
//         emit dropCopyRequested(urls_local);
//         return true;
//         break;

//     case Qt::DropAction::MoveAction:
//         emit dropCutRequested(urls_local);
//         return true;
//         break;

//     default:
//         return false;
//     }

//     return false;
// }