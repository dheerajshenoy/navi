#pragma once

#include <QString>
#include <QFileInfo>
#include <tuple>
#include <QRegularExpression>

class utils {

public:
    utils() = delete;

    static QString getPermString(const QFileInfo &fileInfo) noexcept;
    static std::tuple<bool, uint> isNumber(const QString &numString) noexcept;
    static bool isValidPath(const QString &path) noexcept;
    static QStringList splitPreservingQuotes(const QString& input) noexcept;
    static qint64 parseFileSize(const QString& sizeStr) noexcept;
};