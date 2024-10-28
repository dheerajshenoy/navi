#include "BookmarkManager.hpp"

void BookmarkManager::addBookmark(const QString &bookmarkName,
                                  const QString &itemName) noexcept {}

void BookmarkManager::addBookmarks(const QStringList &bookmarkNames,
                                   const QStringList &itemNames) noexcept {}

void BookmarkManager::removeBookmark(const QString &bookmarkName) noexcept {}

void BookmarkManager::removeBookmarks(const QStringList &bookmarkNames) noexcept {}

Result<bool>
BookmarkManager::getBookmark(const QString &bookmarkName) noexcept {}

bool BookmarkManager::setBookmarkFile(const QString &bookmarkName,
                                      const QString &newPath) noexcept {}

bool BookmarkManager::setBookmarkName(const QString &path,
                                      const QString &newBookmarkName) noexcept {
}

QHash<QString, QString> BookmarkManager::getBookmarks() noexcept {}

void BookmarkManager::clearBookmarks() noexcept {

}