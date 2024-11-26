#pragma once

#include <QString>
#include <QStringList>
#include <QHash>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

class BookmarkManager {

public:
    struct Bookmark {
        QString file_path;
        bool highlight_only = false;
    };

    bool loadBookmarks(const QString &fileName) noexcept;
    bool addBookmark(const QString &bookmarkName,
                     const QString &itemName, const bool &highlightOnly = false) noexcept;
    bool removeBookmark(const QString &bookmarkName) noexcept;
    BookmarkManager::Bookmark getBookmark(const QString &bookmarkName) noexcept;
    QString getBookmarkFilePath(const QString &bookmarkName) noexcept;
    bool setBookmarkFile(const QString &bookmarkName, const QString &newPath,
                         const bool &highlightOnly) noexcept;

    bool setBookmarkName(const QString &bookmarkName,
                         const QString &newBookmarkName) noexcept;
    bool setBookmarkHighlight(const QString &bookmarkName,
                              const bool &highlight) noexcept;
    QHash<QString, Bookmark> getBookmarks() noexcept;
    void clearBookmarks() noexcept;
    void setBookmarks(const QHash<QString, Bookmark> &bookmarksHash) noexcept;

private:
    // bookmarkName, filePath
    QHash<QString, Bookmark> m_bookmarks_hash;
    QString m_bookmarks_file_path;
};