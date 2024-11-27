#include "BookmarkManager.hpp"

bool BookmarkManager::addBookmark(const QString &bookmarkName,
                                  const QString &itemName,
                                  const bool &highlightOnly) noexcept {
    if (m_bookmarks_hash.contains(bookmarkName))
        return false;

    m_bookmarks_hash.insert(bookmarkName,
                            Bookmark{.file_path = itemName, .highlight_only = highlightOnly});

    emit bookmarksChanged();
    m_unsaved_changes = true;

    return true;
}

bool BookmarkManager::removeBookmark(const QString &bookmarkName) noexcept {
    auto t = m_bookmarks_hash.remove(bookmarkName);
    if (t)
        emit bookmarksChanged();
    m_unsaved_changes = true;
    return t;
}

BookmarkManager::Bookmark BookmarkManager::getBookmark(const QString &bookmarkName) noexcept {
    if (m_bookmarks_hash.contains(bookmarkName)) {
        return m_bookmarks_hash[bookmarkName];
    } else {
        return Bookmark();
    }
    return Bookmark();
}

void BookmarkManager::setBookmarks(const QHash<QString, Bookmark> &bookmarks) noexcept {
    m_bookmarks_hash = bookmarks;
    emit bookmarksChanged();
    m_unsaved_changes = true;
}

bool BookmarkManager::setBookmarkFile(const QString &bookmarkName,
                                      const QString &newPath, const bool &highlight) noexcept {
    if (m_bookmarks_hash.contains(bookmarkName)) {
        m_bookmarks_hash.insert(bookmarkName,
                                Bookmark{.file_path = newPath, .highlight_only = highlight});
        emit bookmarksChanged();
        m_unsaved_changes = true;
        return true;
    }

    return false;
}

bool BookmarkManager::setBookmarkName(const QString &oldBookmarkName,
                                      const QString &newBookmarkName) noexcept {
    // If the file does not exist, do nothing
    if (!m_bookmarks_hash.contains(oldBookmarkName))
        return false;

    BookmarkManager::Bookmark oldBookmark = m_bookmarks_hash.value(oldBookmarkName);
    // Get the bookmark pointing file
    QString oldBookmarkFilePath = oldBookmark.file_path;

    // Remove the bookmark with the old name
    m_bookmarks_hash.remove(oldBookmarkName);

    // Add bookmark with the old file but with the new name
    m_bookmarks_hash.insert(newBookmarkName, Bookmark {
                                .file_path = oldBookmarkFilePath,
                                .highlight_only =
              oldBookmark.highlight_only,
    });

    m_unsaved_changes = true;
    emit bookmarksChanged();

    return true;
}

QHash<QString, BookmarkManager::Bookmark> BookmarkManager::getBookmarks() noexcept {
    return m_bookmarks_hash;
}

void BookmarkManager::clearBookmarks() noexcept {
    m_bookmarks_hash.clear();
    m_unsaved_changes = true;
    emit bookmarksChanged();
}

QString BookmarkManager::getBookmarkFilePath(const QString &bookmarkName) noexcept {
    if (!m_bookmarks_hash.contains(bookmarkName))
        return QString();
    return m_bookmarks_hash.value(bookmarkName).file_path;
}