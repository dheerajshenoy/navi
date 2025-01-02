#include "utils.hpp"

QString utils::getPermString(const QFileInfo& fileInfo) noexcept {
    QString permStr;

    if (!fileInfo.exists())
        return QString();

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

QList<StorageDevice> utils::getDrives() noexcept {
    // Function to run lsblk and parse output
    QList<StorageDevice> devices;

    const char *command = "lsblk -o NAME,SIZE,TYPE,FSTYPE,MOUNTPOINT,LABEL -n -r | cut -d ' ' -f1- --output-delimiter=,";
    // Run lsblk command with columns for name, mountpoint, type, and size
    std::array<char, 128> buffer;
    QString result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
                                                  popen(command, "r"), pclose);

    if (!pipe) {
        return {};
    }

    // Read output of lsblk command
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    // Parse each line of lsblk output
    QTextStream stream(&result);
    QString line;
    while (stream.readLineInto(&line)) {
        StorageDevice device;
        line = line.replace("\\x20", " ");
        QStringList ops = line.split(",");
        device.name = ops.at(0);
        device.size = ops.at(1);
        device.type = ops.at(2);
        device.fstype = ops.at(3);
        device.mountPoint = ops.at(4);
        device.label = ops.at(5);

        devices.push_back(device);
    }
    return devices;
}

std::vector<std::string> utils::convertToStdVector(const QStringList& qStringList) noexcept {
    std::vector<std::string> stdVector;
    stdVector.reserve(qStringList.size());
    for (const QString& qStr : qStringList) {
        stdVector.push_back(qStr.toStdString());
    }
    return stdVector;
}

QStringList utils::stringListFromVector(const std::vector<std::string> &vectorList) noexcept {
    QStringList stringList(vectorList.size());

    for (const auto &item : vectorList)
        stringList << QString::fromStdString(item);

    return stringList;
}

QStringList utils::getAssociatedApplications(const QString &mimeType) noexcept {
    QStringList apps;
    QProcess process;
    process.start("sh", QStringList() << "-c" << QString("grep '%1' /usr/share/applications/*.desktop ~/.local/share/applications/*.desktop").arg(mimeType));
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    for (const QString &line : output.split('\n')) {
        if (!line.isEmpty()) {
            QString app = line.section("=", 1, 1); // Extract app name
            apps.append(app);
        }
    }
    return apps;
}


utils::FolderInfo utils::getFolderInfo(const QString &folderPath) noexcept {
    uint32_t count = 0;

    quint64 size = 0;
    QDir dir(folderPath);
    //calculate total size of current directories' files
    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size += fi.size();
        count++;
    }

    //add size of child directories recursively
    QDir::Filters dirFilters =
        QDir::Dirs | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden;

    for(QString childDirPath : dir.entryList(dirFilters)) {
        auto finfo = getFolderInfo(folderPath + QDir::separator() + childDirPath);
        size += finfo.size;
        count += finfo.count;
    }

    FolderInfo finfo;
    finfo.count = count;
    finfo.size = size;

    return finfo;
}

void utils::addTextToFirstLine(QTextEdit *textEdit, const QString &textToAdd) noexcept {
    QTextCursor cursor = textEdit->textCursor(); // Get the current text cursor

    cursor.movePosition(QTextCursor::Start);    // Move to the start of the document
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor); // Select first line

    QString firstLine = cursor.selectedText();  // Get the first line's text
    firstLine = textToAdd + firstLine;          // Prepend new text to the first line

    cursor.insertText(firstLine);               // Replace the first line with updated text
}


// If nlines is -1, read the whole file
QStringList utils::readLinesFromFile(const QString &filename,
                                     const int &nlines) noexcept {
    QStringList lines;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Error opening file:" << file.errorString();
        return QStringList();
    }

    int count = 0;

    if (nlines > -1) {

        while (!file.atEnd() && count < nlines) {
            QString line = file.readLine().trimmed();
            lines.append(line);
            ++count;
        }
    } else {
        while (!file.atEnd()) {
            QString line = file.readLine().trimmed();
            lines.append(line);
            ++count;
        }
    }

    file.close();
    return lines;

}

QString utils::fileName(const QString &path) noexcept {
    return QFileInfo(path).fileName();
}


QString utils::bytes_to_string(const qint64 &bytes) noexcept {
    const char* suffixes[] = { "B", "K", "M", "G", "T", "P" };
    double size = static_cast<double>(bytes);
    int suffixIndex = 0;

    // Iterate over the suffixes and divide the size by 1024
    while (size >= 1024 && suffixIndex < 5) {
        size /= 1024;
        suffixIndex++;
    }

    // Return the formatted string
    return QString::number(size, 'f', 2) + suffixes[suffixIndex];
}

QString utils::joinPaths(const QString& first) noexcept {
    return first;
}

template <typename... Args>
QString utils::joinPaths(const QString& first, const Args&... args) noexcept {
    QString rest = joinPaths(args...);
    if (first.endsWith("/") && rest.startsWith("/")) {
        return first + rest.mid(1); // Remove extra slash
    } else if (!first.endsWith("/") && !rest.startsWith("/")) {
        return first + "/" + rest; // Add a slash between
    } else {
        return first + rest; // Already properly joined
    }
}

// Helper function to read a binary file
std::vector<char> utils::readBinaryFile(const std::string &filePath) noexcept {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        return {};
    }

    return buffer;
}


// Recursive function to resolve all members of a Lua table
void utils::getTableMembers(sol::table tbl, const std::string& prefix,
                            std::vector<std::string>& results) noexcept {
    for (const auto& pair : tbl) {
        sol::object key = pair.first;
        sol::object value = pair.second;

        // Construct the full name
        std::string fullName = prefix + key.as<std::string>();

        if (value.get_type() == sol::type::table) {
            // If the value is a table, recurse into it
            getTableMembers(value.as<sol::table>(), fullName + ".", results);
        } else {
            // Add the fully resolved name to results
            results.push_back(fullName);
        }
    }
}
