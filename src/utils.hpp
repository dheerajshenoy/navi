#pragma once

#include <QString>
#include <QFileInfo>
#include <tuple>
#include <QRegularExpression>
#include <QList>
#include <QProcess>
#include <QDir>
#include <QTextEdit>
#include "StorageDevice.hpp"

class utils {

public:
    utils() = delete;

    struct FolderInfo {
        quint64 size;
        uint32_t count;
    };

    static QString getPermString(const QFileInfo &fileInfo) noexcept;
    static std::tuple<bool, uint> isNumber(const QString &numString) noexcept;
    static bool isValidPath(const QString &path) noexcept;
    static QStringList splitPreservingQuotes(const QString& input) noexcept;
    static qint64 parseFileSize(const QString& sizeStr) noexcept;
    static QList<StorageDevice> getDrives() noexcept;
    static bool mountDrive(const QString &driveName) noexcept;
    static std::vector<std::string> convertToStdVector(const QStringList &qStringList) noexcept;
    static QStringList stringListFromVector(const std::vector<std::string> &vectorList) noexcept;
    static bool copyFile(const QString &sourceFile, const QString &destFile) noexcept;
    static QStringList getAssociatedApplications(const QString &mimeType) noexcept;

    static FolderInfo getFolderInfo(const QString &path) noexcept;
    static void addTextToFirstLine(QTextEdit *textedit,
                                   const QString &text) noexcept;
    static QStringList readLinesFromFile(const QString &filename, const int &lines) noexcept;

    static QString fileName(const QString &path) noexcept;
};
