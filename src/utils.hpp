#pragma once

#include "pch/pch_utils.hpp"

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
    static QString bytes_to_string(const qint64 &bytes) noexcept;


    static QString joinPaths(const QString& first) noexcept;

    template <typename... Args>
    static QString joinPaths(const QString& first, const Args&... args) noexcept;

    static std::vector<char> readBinaryFile(const std::string &filePath) noexcept;


    static void getTableMembers(sol::table tbl, const std::string& prefix,
                                std::vector<std::string>& results) noexcept;

    static void getTableMembers(sol::table tbl, const std::string& prefix,
                         const std::vector<std::string> &result) noexcept;

    static QString getInput(QWidget *parent,
                         const QString &title,
                         const QString &text,
                         const QString &default_text,
                         const QString &selection_text) noexcept;

};
