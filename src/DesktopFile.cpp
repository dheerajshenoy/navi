#include "DesktopFile.hpp"
#include "AboutWidget.hpp"
#include <QFile>
#include <QDebug>

DesktopFile::DesktopFile(const QString &fileName) {

    // Store file name
    this->fileName = joinPaths(DESKTOP_FILE_DIR, fileName);

    // File validity
    if (!QFile::exists(this->fileName)) {
        return;
    }

    // Loads .desktop file (read from 'Desktop Entry' group)
    read_desktop_file();

    name = m_desktop_hash["Name"].toString();
    exec = m_desktop_hash["Exec"].toString();
    icon = m_desktop_hash["Icon"].toString();
    type = m_desktop_hash["Type"].toString();
    no_display = m_desktop_hash["NoDisplay"].toBool();
    terminal = m_desktop_hash["Terminal"].toBool();
    categories = m_desktop_hash["Categories"].toString().remove(" ").split(";", Qt::SkipEmptyParts);
    mimeType = m_desktop_hash["MimeType"].toString().remove(" ").split(";", Qt::SkipEmptyParts);
    genericName = m_desktop_hash["GenericName"].toString();
}

QString DesktopFile::getFileName() const noexcept {
    return fileName;
}

QString DesktopFile::getPureFileName() const noexcept {
    return fileName.split("/").last().remove(".desktop");
}

QString DesktopFile::getName() const noexcept {
    return name;
}

QString DesktopFile::getGenericName() const noexcept {
    return genericName;
}

QString DesktopFile::getExec() const noexcept {
    return exec;
}

QString DesktopFile::getIcon() const noexcept {
    return icon;
}

QString DesktopFile::getType() const noexcept {
    return type;
}

bool DesktopFile::noDisplay() const noexcept {
    return no_display;
}

bool DesktopFile::isTerminal() const noexcept {
    return terminal;
}

QStringList DesktopFile::getCategories() const noexcept {
    return categories;
}

QStringList DesktopFile::getMimeType() const noexcept {
    return mimeType;
}

void DesktopFile::read_desktop_file() noexcept {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString group = "Desktop Entry";

    // Indicator whether group was found or not, if name of group was not
    // specified, groupFound is always true
    bool groupFound = group.isEmpty();

    // Read properties
    QTextStream in(&file);
    while (!in.atEnd()) {

        // Read new line
        QString line = in.readLine();

        // Skip empty line or line with invalid format
        if (line.trimmed().isEmpty()) {
            continue;
        }

        // Read group
        // NOTE: symbols '[' and ']' can be found not only in group names, but
        // only group can start with '['
        if (!group.isEmpty() && line.trimmed().startsWith("[")) {
            QString tmp = line.trimmed().replace("[", "").replace("]", "");
            groupFound = group.trimmed().compare(tmp) == 0;
        }

        // If we are in correct group and line contains assignment then read data
        if (groupFound && line.contains("=")) {
            int index = line.indexOf("=");
            QString key = line.mid(0, index);
            QString val = line.mid(index+1);
            m_desktop_hash[key] = val;
        }
    }

    file.close();
}
