#include "BookmarkManager.hpp"

bool BookmarkManager::loadBookmarks(const QString &filePath) noexcept {
    QFile bookmarkFile(filePath);

    if (!bookmarkFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    m_bookmarks_file_path = filePath;

    QTextStream in(&bookmarkFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("#") || line.isEmpty())
            continue;

        QStringList bookmarkItem = line.split(QRegularExpression("\\s*,\\s*"), Qt::SkipEmptyParts);
        m_bookmarks_hash[bookmarkItem.at(0)] = bookmarkItem.at(1);
    }

    bookmarkFile.close();
    return true;
}

bool BookmarkManager::addBookmark(const QString &bookmarkName,
                                  const QString &itemName) noexcept {
    if (m_bookmarks_hash.contains(bookmarkName))
        return false;

    m_bookmarks_hash.insert(bookmarkName, itemName);
    return true;
}

bool BookmarkManager::removeBookmark(const QString &bookmarkName) noexcept {
    return m_bookmarks_hash.remove(bookmarkName);
}

QString BookmarkManager::getBookmark(const QString &bookmarkName) noexcept {
    if (m_bookmarks_hash.contains(bookmarkName)) {
        return m_bookmarks_hash.value(bookmarkName);
    } else {
        return QString();
    }
}

bool BookmarkManager::setBookmarkFile(const QString &bookmarkName,
                                      const QString &newPath) noexcept {
    if (m_bookmarks_hash.contains(bookmarkName)) {
        m_bookmarks_hash.remove(bookmarkName);
        m_bookmarks_hash.insert(bookmarkName, newPath);
        return true;
    }

    return false;
}

bool BookmarkManager::setBookmarkName(const QString &oldBookmarkName,
                                      const QString &newBookmarkName) noexcept {
    // If the file does not exist, do nothing
    if (!m_bookmarks_hash.contains(oldBookmarkName))
        return false;

    // Get the bookmark pointing file
    QString oldBookmarkFilePath = m_bookmarks_hash.value(oldBookmarkName);

    // Remove the bookmark with the old name
    m_bookmarks_hash.remove(oldBookmarkName);

    // Add bookmark with the old file but with the new name
    m_bookmarks_hash.insert(newBookmarkName, oldBookmarkFilePath);

    return true;
}

QHash<QString, QString> BookmarkManager::getBookmarks() noexcept {}

void BookmarkManager::clearBookmarks() noexcept {}

bool BookmarkManager::saveBookmarksFile() noexcept {
    QFile file(m_bookmarks_file_path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    for (auto i = m_bookmarks_hash.cbegin(), end = m_bookmarks_hash.cend();
         i != end; i++) {
        file.write(QString("%1,%2").arg(i.key()).arg(i.value()).toStdString().c_str());
    }
    return true;
}

QString
BookmarkManager::getBookmarkFilePath(const QString &bookmarkName) noexcept {
    if (!m_bookmarks_hash.contains(bookmarkName))
        return QString();
    return m_bookmarks_hash.value(bookmarkName);
}