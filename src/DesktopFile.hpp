#pragma once

#include <QStringList>
#include <QHash>
#include "Globals.hpp"

class DesktopFile {
public:
    explicit DesktopFile(const QString &fileName = "");
    QString getFileName() const noexcept;
    QString getPureFileName() const noexcept;
    QString getName() const noexcept;
    QString getGenericName() const noexcept;
    QString getExec() const noexcept;
    QString getIcon() const noexcept;
    QString getType() const noexcept;
    bool noDisplay() const noexcept;
    bool isTerminal() const noexcept;
    QStringList getCategories() const noexcept;
    QStringList getMimeType() const noexcept;
    void read_desktop_file() noexcept;

private:
    QString fileName;
    QString name;
    QString genericName;
    QString exec;
    QString icon;
    QString type;
    bool no_display;
    bool terminal;
    QStringList categories;
    QStringList mimeType;
    QHash<QString, QVariant> m_desktop_hash;

};

