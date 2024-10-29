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