#pragma once

#include <QString>
#include <QStringList>
#include <QHash>
#include "Result.hpp"

class BookmarkManager {

public:
    void addBookmark(const QString &bookmarkName,
                     const QString &itemName) noexcept;
    void addBookmarks(const QStringList &bookmarkNames,
                      const QStringList &itemNames) noexcept;
    void removeBookmark(const QString &bookmarkName) noexcept;
    void removeBookmarks(const QStringList &bookmarkNames) noexcept;
    Result<bool> getBookmark(const QString &bookmarkName) noexcept;
    bool setBookmarkFile(const QString &bookmarkName, const QString &newPath) noexcept;
    bool setBookmarkName(const QString &path, const QString &newBookmarkName) noexcept;
    QHash<QString, QString> getBookmarks() noexcept;
    void clearBookmarks() noexcept;

private:
    QHash<QString, QString> m_bookmarks;
};