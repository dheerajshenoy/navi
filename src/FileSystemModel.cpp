#include "FileSystemModel.hpp"

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

    m_file_name_column_index = indexOfFileNameColumn();
}

int FileSystemModel::indexOfFileNameColumn() const noexcept {
    for (std::size_t i = 0; i < m_column_list.size(); i++)
        if (m_column_list.at(i).type == ColumnType::FileName)
            return i;
    return -1;
}

void FileSystemModel::clearMarkedFilesListLocal() noexcept {
    /*this->beginResetModel();*/
    auto marksHere = getMarkedFilesLocal();
    for (const auto &file : marksHere) {
        QModelIndex index = getIndexFromString(file);
        setData(index, false, static_cast<int>(Role::Marked));
        m_markedFiles.remove(file);
    }
    /*this->endResetModel();*/
    emit marksListChanged();
}

uint FileSystemModel::getMarkedFilesCount() noexcept {
    return m_markedFiles.size();
}

uint FileSystemModel::getMarkedFilesCountLocal() noexcept {
    return getMarkedFilesLocal().size();
}

void FileSystemModel::setRootPath(const QString &path) noexcept {
    m_file_system_watcher->removePath(m_root_path);
    m_root_path = path;
    loadDirectory(path, m_dir_sort_flags);
    m_file_system_watcher->addPath(path);
}

QString FileSystemModel::filePath(const QModelIndex &index) noexcept {
    if (!index.isValid())
        return QString();

    return m_root_path + QDir::separator() +
        index.data(static_cast<int>(FileSystemModel::Role::FilePath))
        .toString();
}

bool FileSystemModel::isDir(const QModelIndex &index) noexcept {
    if (!index.isValid())
        return false;

    // Assuming `fileInfoList` holds QFileInfo objects or a similar structure
    return QFileInfo(index.data(static_cast<int>(FileSystemModel::Role::FilePath)).toString()).isDir();
}

int FileSystemModel::findRow(const QFileInfo &fileInfo) const noexcept {
    return m_path_row_hash.value(fileInfo.absoluteFilePath(),
                                 -1); // Returns -1 if path not found
}

int FileSystemModel::findRow(const QString &path) const noexcept {
    return m_path_row_hash.value(path, -1); // Returns -1 if path not found
}

QModelIndex FileSystemModel::index(const QString &path) const noexcept {
    int row = findRow(path);
    if (row == -1)
        return QModelIndex(); // Return invalid index if not found

    // Assuming `fileInfoList` is organized with parent-child relationships
    return createIndex(row, 0);
}

void FileSystemModel::loadDirectory(const QString &path,
                                    const QDir::SortFlags &sort_flags) noexcept {
    beginResetModel();
    m_all_file_paths.clear();
    m_file_paths.clear();
    m_path_row_hash.clear();
    m_loaded_files_count = 0;

    // Pre-allocate the necessary space
    m_all_file_paths.reserve(1000); // Assuming the number of files/directories is large, adjust as needed
    m_path_row_hash.reserve(1000);  // Adjust for expected number of entries

    QStringList directories, files;
    int row = 0;

    // Create a QDirIterator for both directories and files
    QDirIterator it(path, m_name_filters, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::NoIteratorFlags);

    while (it.hasNext()) {
        QString filePath = it.next();
        if (it.fileInfo().isDir()) {
            directories.append(filePath);  // Directories first
        } else {
            files.append(filePath);  // Files next
        }
    }

    QStringList first, second;
    // Sort directories and files based on the flag
    if (sort_flags & QDir::SortFlag::DirsFirst) {
        first = directories;
        second = files;
    } else {
        first = files;
        second = directories;
    }

    // Append the sorted lists to the final model
    for (const QString &filePath : first) {
        m_all_file_paths.append(filePath);
        m_path_row_hash.insert(filePath, row++);
    }

    for (const QString &filePath : second) {
        m_all_file_paths.append(filePath);
        m_path_row_hash.insert(filePath, row++);
    }

    qDebug() << first;
    qDebug() << "------------------";
    qDebug() << second;

    m_total_files_count = m_all_file_paths.size();
    endResetModel();

    fetchMore(QModelIndex());
    emit directoryLoaded(m_total_files_count);
}

// int FileSystemModel::rowCount(const QModelIndex &parent) const {
//     return parent.isValid() ? 0 : m_file_paths.size();
// }

int FileSystemModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_loaded_files_count;  // Only return the number of currently loaded files
}

int FileSystemModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid()
        ? 0
        : m_column_list.size(); // Three columns: Name, Size, Last Modified
}

void FileSystemModel::setMarkForegroundColor(const QString &color) noexcept {
    if (color.isEmpty() || color.isNull())
        m_markForegroundColor = QColor(Qt::transparent);
    else
        m_markForegroundColor = QColor(color);
}

void FileSystemModel::setMarkBackgroundColor(const QString &color) noexcept {
    m_markBackgroundColor = QColor(color);
}

void FileSystemModel::setMarkHeaderForegroundColor(const QString &color) noexcept {
    m_markHeaderForegroundColor = QColor(color);
}

void FileSystemModel::setMarkHeaderBackgroundColor(const QString &color) noexcept {
    m_markHeaderBackgroundColor = QColor(color);
}

void FileSystemModel::setMarkHeaderFontBold(const bool &state) noexcept {
    m_markHeaderFont.setBold(state);
}

void FileSystemModel::setMarkHeaderFontItalic(const bool &state) noexcept {
    m_markHeaderFont.setItalic(state);
}

void FileSystemModel::setMarkFontBold(const bool &state) noexcept {
    m_markFont.setBold(state);
}

void FileSystemModel::setMarkFontItalic(const bool &state) noexcept {
    m_markFont.setItalic(state);
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_loaded_files_count)
        return QVariant();

    const QFileInfo fileInfo(m_file_paths.at(index.row()));
    // Custom background color for marked files
    switch (role) {

    case static_cast<int>(Role::Marked):
        return m_markedFiles.contains(index.data(static_cast<int>(Role::FilePath)).toString());
        break;

    case Qt::DecorationRole: {
        if (icons_enabled) {
            if (index.column() == static_cast<int>(ColumnType::FileName)) {
                return get_cached_icon(fileInfo);
            }
        }
    } break;

    case Qt::ForegroundRole: {
        bool isMarked = m_markedFiles.contains(getPathFromIndex(index.siblingAtColumn(m_file_name_column_index)));
        if (isMarked) {
            return m_markForegroundColor;
        }
        return QVariant();
    } break;

    case Qt::BackgroundRole: {
        bool isMarked = m_markedFiles.contains(index.data(static_cast<int>(Role::FilePath)).toString());
        if (isMarked) {
            return m_markBackgroundColor;
        }


    } break;

    case Qt::FontRole: {
        bool isMarked = m_markedFiles.contains(index.data(static_cast<int>(Role::FilePath)).toString());
        if (isMarked) {
            return m_markFont;
        }
    } break;

    case static_cast<int>(FileSystemModel::Role::FilePath):
        return fileInfo.filePath();
        break;

    case static_cast<int>(FileSystemModel::Role::FileName):
        return fileInfo.fileName();
        break;

    case static_cast<int>(FileSystemModel::Role::Symlink):
        return fileInfo.symLinkTarget();
        break;

    case Qt::DisplayRole: {
        switch (m_column_list.at(index.column()).type) {
        case ColumnType::FileName: {
            if (fileInfo.isSymbolicLink()) {
                if (m_show_symlink) {
                    return QString("%1 %2 %3")
                        .arg(fileInfo.fileName())
                        .arg(m_symlink_separator)
                        .arg(fileInfo.symLinkTarget());
                } else
                    return fileInfo.fileName();
            } else
                return fileInfo.fileName();
        } break;

        case ColumnType::FileSize: // File Size
            return fileInfo.isDir()
                ? QVariant()
                : QVariant(m_locale.formattedDataSize(fileInfo.size()));

        case ColumnType::FileModifiedDate: // Last Modified Date
            return fileInfo.lastModified().toString("yyyy-MM-dd HH:mm:ss");

        case ColumnType::FilePermission:
            return utils::getPermString(fileInfo);

        default:
            return QVariant();
        }
    }

    case static_cast<int>(Role::Cursor):
        return m_cursor_row;
    }

    return QVariant();
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const {
    if (orientation == Qt::Vertical) {
        switch (role) {

        case Qt::BackgroundRole: {
            bool isMarked = m_markedFiles.contains(getPathFromRow(section));
            if (isMarked)
                return m_markHeaderBackgroundColor;
        } break;

        case Qt::ForegroundRole: {
            bool isMarked = m_markedFiles.contains(getPathFromRow(section));
            if (isMarked)
                return m_markHeaderForegroundColor;
        } break;

        case Qt::FontRole: {
            bool isMarked = m_markedFiles.contains(getPathFromRow(section));
            if (isMarked) {
                return m_markHeaderFont;
            }
        } break;

        case Qt::DisplayRole:
            return section;
            break;

        default:
            return QVariant();
            break;
        }

    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_column_list.at(section).name;
    }

    return QVariant();
}

bool FileSystemModel::setData(const QModelIndex &index, const QVariant &value,
                              int role) {
    if (role == static_cast<int>(Role::Marked)) {
        auto v = value.toBool();
        if (v == true) {
            m_markedFiles.insert(index.data(static_cast<int>(Role::FilePath)).toString());
        } else if (v == false) {
            m_markedFiles.remove(index.data(static_cast<int>(Role::FilePath)).toString());
        }
        emit dataChanged(index, index, {role});
        emit headerDataChanged(Qt::Vertical, index.row(), index.row());
        emit marksListChanged();
        return true;
    }

    return false;
}

void FileSystemModel::setFilter(const QDir::Filters &filters) noexcept {
    m_dir_filters = filters;
}

QModelIndex FileSystemModel::index(int row, int col) const noexcept {
    if (row < 0 || row >= m_file_paths.size() || col < 0 ||
        col >= columnCount()) {
        return QModelIndex(); // Return an invalid index if out of bounds
    }

    // Create the index using the row, column, and a pointer to the QFileInfo
    // object for that row
    return createIndex(row, col);
}

bool FileSystemModel::removeMarkedFile(const QString &path) noexcept {
    if (m_markedFiles.contains(path)) {
        if (m_markedFiles.remove(path)) {
            emit marksListChanged();
            return true;
        }
    }
    return false;
}

void FileSystemModel::removeMarkedFiles(const QStringList &paths) noexcept {
    if (m_markedFiles.isEmpty())
        return;

    for (const auto &mark : paths) {
        if (m_markedFiles.contains(mark)) {
            QModelIndex index = getIndexFromString(mark);
            if (index.isValid()) {
                setData(index, false, static_cast<int>(Role::Marked));
            }

            m_markedFiles.remove(mark);
            emit marksListChanged();
        }
    }
}

void FileSystemModel::removeMarkedFiles() noexcept {
    for (const auto &mark : m_markedFiles) {
        QModelIndex index = getIndexFromString(mark);
        setData(index, false, static_cast<int>(Role::Marked));
    }
    m_markedFiles.clear();
    emit marksListChanged();
}

QModelIndex
FileSystemModel::getIndexFromString(const QString &path) const noexcept {
    // Iterate through m_fileInfoList to find the file info matching the given
    int pos = m_file_paths.indexOf(path);
    if (pos == -1)
        return QModelIndex(); // Return an invalid index if not found

    return createIndex(pos, 0);
}

QModelIndexList FileSystemModel::getIndexesFromStrings(const QStringList &paths) const noexcept {

    QModelIndexList indexList(paths.size());
    for (int i = 0; i < m_file_paths.size(); ++i) {
        for (int j = 0; j < paths.size(); j++) {
            QString fileName = QFileInfo(m_file_paths.at(i)).fileName();
            if (paths.at(j) == fileName)
                indexList.push_back(index(j, 0));
        }
    }

    return indexList;
}

QModelIndex FileSystemModel::getIndexFromBaseName(const QString &path) const noexcept {
    // Iterate through m_fileInfoList to find the file info matching the given
    // path
    for (int i = 0; i < m_file_paths.size(); ++i) {
        if (QFileInfo(m_file_paths.at(i)).fileName() == path) {
            return index(i, 0); // Return the index for the matching item
        }
    }


    return QModelIndex(); // Return an invalid index if not found
}

int FileSystemModel::getRowFromBaseName(const QString &path) const noexcept {

    for (int i=0; i < m_file_paths.size(); ++i)
        if (QFileInfo(m_file_paths.at(i)).fileName() == path)
            return i;

    return -1;
}

bool FileSystemModel::removeMarkedFile(const QModelIndex &index) noexcept {
    QString path = getPathFromIndex(index);
    if (m_markedFiles.contains(path) && m_markedFiles.remove(path)) {
        setData(index, false, static_cast<int>(Role::Marked));
        emit marksListChanged();
        return true;
    }
    return false;
}

void FileSystemModel::setNameFilters(const QStringList &name_filters) noexcept {
    m_name_filters = name_filters;
}

QList<QModelIndex> FileSystemModel::match(const QModelIndex &start, int role,
                                          const QVariant &value, int hits,
                                          Qt::MatchFlags flags) const {

    QList<QModelIndex> matchedIndexes;
    // Define search parameters based on provided start index
    int startRow = start.isValid() ? start.row() : 0;
    int startColumn = start.isValid() ? start.column() : 0;

    int rows = rowCount();
    int columns = columnCount();


    if (flags & Qt::MatchRegularExpression) {
        // Convert the search value to a QRegularExpression
        QRegularExpression regex(value.toString(), QRegularExpression::PatternOption::CaseInsensitiveOption);
        if (!regex.isValid()) {
            return matchedIndexes; // Return empty list if regex is invalid
        }

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
    } else if (flags & (~Qt::MatchRegularExpression)) {
        // Loop through rows and columns to find matching items
        for (int row = startRow; row < rows; ++row) {
            for (int col = startColumn; col < columns; ++col) {
                QModelIndex _index = index(row, col);
                if (_index.isValid()) {
                    QVariant _data = data(_index, role);

                    // Check if data matches the regex
                    if (_data.toString().contains(value.toString(), Qt::CaseInsensitive)) {
                        matchedIndexes.append(_index);

                        // Stop if we've reached the desired number of hits
                        if (hits > 0 && matchedIndexes.size() >= hits) {
                            return matchedIndexes;
                        }
                    }
                }
            }
        }
    }

    return matchedIndexes;
}

QStringList FileSystemModel::getMarkedFilesLocal() noexcept {

    // Return {} if no marks are found in the global level
    if (m_markedFiles.isEmpty())
        return QStringList();

    QStringList markedFilesLocal;
    for (const auto &file : m_markedFiles) {
        if (file.contains(m_root_path))
            markedFilesLocal.append(file);
    }

    return markedFilesLocal;
}

bool FileSystemModel::hasMarksLocal() noexcept {
    // Return {} if no marks are found in the global level
    if (m_markedFiles.isEmpty())
        return false;

    for (const auto &file : m_markedFiles) {
        if (file.contains(m_root_path))
            return true;
    }

    return false;
}

QStringList FileSystemModel::getMarkedFiles() noexcept {

    // Return {} if no marks are found in the global level
    if (m_markedFiles.isEmpty())
        return QStringList();

    return QStringList(m_markedFiles.cbegin(), m_markedFiles.cend());
}

QModelIndexList FileSystemModel::getMarkedFilesIndexes() noexcept {

    // Return {} if no marks are found in the global level
    if (m_markedFiles.isEmpty())
        return QModelIndexList();

    QModelIndexList list;
    list.reserve(m_markedFiles.size());

    for (const auto &file : m_markedFiles) {
        QModelIndex index = getIndexFromString(file);
        list.append(index);
    }

    return list;
}

void FileSystemModel::setColumns(const QList<Column> &cols) noexcept {
    m_column_list.clear();

    for (const auto &col : cols)
        m_column_list.push_back(col);

    m_file_name_column_index = indexOfFileNameColumn();

    emit headerDataChanged(Qt::Orientation::Horizontal, 0, -1);
}

void FileSystemModel::setMarkFontFamily(const QString &family) noexcept {
    m_markFont.setFamily(family);
}

void FileSystemModel::setMarkHeaderFontFamily(const QString &family) noexcept {
    m_markHeaderFont.setFamily(family);
}

void FileSystemModel::setSymlinkSeparator(const QString &separator) noexcept {
    m_symlink_separator = separator;
}

void FileSystemModel::setSortBy(const QDir::SortFlags &sortFlags) noexcept {
    m_dir_sort_flags = sortFlags;
    loadDirectory(m_root_path, m_dir_sort_flags);
}

void FileSystemModel::addDirFilter(const QDir::Filters &filter) noexcept {
    if (!(m_dir_filters & filter))
        m_dir_filters |= filter;
}

void FileSystemModel::removeDirFilter(const QDir::Filters &filter) noexcept {
    if (m_dir_filters & filter)
        m_dir_filters &= ~filter;
}


QStringList FileSystemModel::get_columns() noexcept {
    QStringList list;
    list.reserve(m_column_list.size());

    for (int i=0; i < m_column_list.size(); i++)
        list[i] = m_column_list.at(i).name;

    return list;
}

QIcon FileSystemModel::get_cached_icon(const QFileInfo &finfo) const {
    QString path = finfo.filePath();
    if (!m_icon_cache.contains(path)) {
        QFileIconProvider iconProvider;
        m_icon_cache[path] = iconProvider.icon(finfo);
    }

    return m_icon_cache[path];
}

QStringList FileSystemModel::getFilePathsFromIndexList(const QModelIndexList &indexList) const noexcept {
    QStringList stringList;
    stringList.reserve(indexList.size());
    for (const auto &index : indexList) {
        stringList.append(getPathFromIndex(index));
    }
    return stringList;
}


QStringList FileSystemModel::get_file_paths_from_rows(const QList<int> &rowList) const noexcept {
    QStringList stringList;
    stringList.reserve(rowList.size());
    for (const auto &row : rowList) {
        stringList.append(getPathFromRow(row));
    }
    return stringList;
}


void FileSystemModel::fetchMore(const QModelIndex &parent) {
    if (parent.isValid() || m_loaded_files_count >= m_total_files_count)
        return;

    const int itemsToLoad = 25;  // Load files in batches
    int itemsToFetch = qMin(itemsToLoad, m_total_files_count - m_loaded_files_count);

    beginInsertRows(QModelIndex(), m_loaded_files_count, m_loaded_files_count + itemsToFetch - 1);
    for (int i = 0; i < itemsToFetch; ++i) {
        QString filePath = m_all_file_paths[m_loaded_files_count];
        m_file_paths.append(filePath);
        m_path_row_hash.insert(filePath, m_loaded_files_count);
        ++m_loaded_files_count;
    }
    endInsertRows();
}

bool FileSystemModel::canFetchMore(const QModelIndex &parent) const {
    if (parent.isValid())
        return false;
    return m_loaded_files_count < m_total_files_count;
}
