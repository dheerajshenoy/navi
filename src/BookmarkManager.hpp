#pragma once

#include <QString>
#include <QStringList>
#include <QHash>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

class BookmarkManager {

public:
    bool loadBookmarks(const QString &fileName) noexcept;
    bool addBookmark(const QString &bookmarkName,
                     const QString &itemName) noexcept;
    bool removeBookmark(const QString &bookmarkName) noexcept;
    QString getBookmark(const QString &bookmarkName) noexcept;
    QString getBookmarkFilePath(const QString &bookmarkName) noexcept;
    bool setBookmarkFile(const QString &bookmarkName, const QString &newPath) noexcept;
    bool setBookmarkName(const QString &bookmarkName, const QString &newBookmarkName) noexcept;
    QHash<QString, QString> getBookmarks() noexcept;
    void clearBookmarks() noexcept;
    bool saveBookmarksFile() noexcept;

private:
    // bookmarkName, filePath
    QHash<QString, QString> m_bookmarks_hash;
    QString m_bookmarks_file_path;
};