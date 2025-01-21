#pragma once

#include <QStandardPaths>
#include <QDesktopServices>
#include <QDir>

inline QString joinPaths(const QString& first) noexcept {
    return first;
}

template <typename... Args>
inline QString joinPaths(const QString& first, const Args&... args) noexcept {
    QString rest = joinPaths(args...);
    if (first.endsWith("/") && rest.startsWith("/")) {
        return first + rest.mid(1); // Remove extra slash
    } else if (!first.endsWith("/") && !rest.startsWith("/")) {
        return first + "/" + rest; // Add a slash between
    } else {
        return first + rest; // Already properly joined
    }
}

static const QString APP_NAME = "navi";
static const QString CONFIG_DIR_PATH = joinPaths(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation), APP_NAME);
static const QString CONFIG_FILE_NAME = "init.lua";
static const QString CONFIG_FILE_PATH = joinPaths(CONFIG_DIR_PATH, CONFIG_FILE_NAME);
static const QString LUA_DIR_NAME = "lua";
static const QString LUA_DIR_PATH = joinPaths(CONFIG_DIR_PATH, LUA_DIR_NAME);
static const QString BOOKMARK_FILE_NAME = "bookmark.lua";
static const QString BOOKMARK_FILE_PATH = joinPaths(CONFIG_DIR_PATH, BOOKMARK_FILE_NAME);

static const QString PRIVATE_API_FILE_NAME = "navi.luac";
static const QStringList PRIVATE_API_FOLDER_PATH = { joinPaths("/usr/share", APP_NAME),
    joinPaths(QDir::homePath(), ".local/share", APP_NAME) };
static const QStringList PRIVATE_API_FILE_PATH = { joinPaths( PRIVATE_API_FOLDER_PATH.at(0), "_lua", PRIVATE_API_FILE_NAME),
    joinPaths(QDir::homePath(), ".local/share", APP_NAME, "_lua", PRIVATE_API_FILE_NAME) };


static const QString MIME_CACHE_FILE_PATH = joinPaths(CONFIG_DIR_PATH, "mime.cache");
