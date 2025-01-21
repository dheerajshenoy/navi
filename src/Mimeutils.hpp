#pragma once

#include "pch_mimeutils.hpp"
#include "Globals.hpp"

class MimeUtils : public QObject {
    Q_OBJECT
    public:
    explicit MimeUtils(QObject* parent = Q_NULLPTR);
    virtual ~MimeUtils();
    void readAssociationFile() noexcept;
    void open_in_app(const QString &file, const QString &app) noexcept;
    void open_files_in_app(const QStringList &files, const QString &app) noexcept;
    void setDefault(const QString &mime, const QStringList &apps);
    QStringList getDefault(const QString &mime) const;
    QStringList getMimeTypes() const;
    QString getMimeType(const QString &path);
    QString getAppForMimeType(const QString &mime) const;

public slots:
    void generateDefaults();
    void saveDefaults();
    void loadDefaults();
private:
    bool m_defaultsChanged = false;
    QStringList mimeList = {
        "~/.local/share/applications/mimeapps.list",
        "/usr/share/applications/mimeapps.list",
        "~/.config/mimeapps.list"
    };

    QHash<QString, QStringList> m_association_hash;
};
