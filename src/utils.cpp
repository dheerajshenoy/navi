#include "utils.hpp"

QString utils::getPermString(const QFileInfo& fileInfo) noexcept {
    QString permStr;

    // File type (directory or file)
    permStr += fileInfo.isDir() ? 'd' : '-';
    QFile::Permissions permissions = fileInfo.permissions();

    // Owner permissions
    permStr += (permissions & QFile::ReadOwner) ? 'r' : '-';
    permStr += (permissions & QFile::WriteOwner) ? 'w' : '-';
    permStr += (permissions & QFile::ExeOwner) ? 'x' : '-';

    // Group permissions
    permStr += (permissions & QFile::ReadGroup) ? 'r' : '-';
    permStr += (permissions & QFile::WriteGroup) ? 'w' : '-';
    permStr += (permissions & QFile::ExeGroup) ? 'x' : '-';

    // Others permissions
    permStr += (permissions & QFile::ReadOther) ? 'r' : '-';
    permStr += (permissions & QFile::WriteOther) ? 'w' : '-';
    permStr += (permissions & QFile::ExeOther) ? 'x' : '-';

    return permStr;
}

std::tuple<bool, uint> utils::isNumber(const QString &numString) noexcept {
    bool ok;
    auto num = numString.toUInt(&ok);
    return std::tuple<bool, uint>(ok, num);
}

bool utils::isValidPath(const QString &path) noexcept {
    QFileInfo file(path);
    return file.exists() && file.isReadable();
}

QStringList utils::splitPreservingQuotes(const QString& input) noexcept {
    QRegularExpression regex("\"([^\"]*)\"|(\\S+)");
    QStringList result;

    QRegularExpressionMatchIterator it = regex.globalMatch(input);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        // If the quoted group matched, use it; otherwise, use the full match
        result.append(match.captured(1).isEmpty() ? match.captured(0) : match.captured(1));
    }
    return result;
}

qint64 utils::parseFileSize(const QString& sizeStr) noexcept {
    // Define the multipliers for each suffix
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    const qint64 TB = GB * 1024;
    const qint64 PB = TB * 1024;
    const qint64 EB = PB * 1024;

    // Regular expression to match the size format, e.g., "10M", "5G", "100K", "1T", "500B"
    QRegularExpression regex(R"((\d+(?:\.\d*)?)\s*([KMGTP]?[B]?)?)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = regex.match(sizeStr.trimmed());

    // Check if the input matches the pattern
    if (!match.hasMatch()) {
        return -1; // Return an error value or handle invalid input as needed
    }

    // Extract the number part and the optional suffix
    QString numberPart = match.captured(1);
    QString suffix = match.captured(2).toUpper();

    bool ok;
    double value = numberPart.toDouble(&ok);
    if (!ok) {
        return -1; // Return an error if conversion fails
    }

    // Apply the multiplier based on the suffix
    if (suffix == "K" || suffix == "KB") {
        return static_cast<qint64>(value * KB);
    } else if (suffix == "M" || suffix == "MB") {
        return static_cast<qint64>(value * MB);
    } else if (suffix == "G" || suffix == "GB") {
        return static_cast<qint64>(value * GB);
    } else if (suffix == "T" || suffix == "TB") {
        return static_cast<qint64>(value * TB);
    } else if (suffix == "P" || suffix == "PB") {
        return static_cast<qint64>(value * PB);
    } else if (suffix == "E" || suffix == "EB") {
        return static_cast<qint64>(value * EB);
    } else if (suffix == "" || suffix == "B") {
        return static_cast<qint64>(value); // Assume bytes if no suffix or "B" suffix
    }

    return -1; // Return an error if the suffix is unrecognized
}