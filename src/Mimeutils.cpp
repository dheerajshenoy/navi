#include "Mimeutils.hpp"
#define MIME_FILE_NAME QDir::homePath() + QDir::separator() + ".config/mimeapps.list"

MimeUtils::MimeUtils(QObject *parent) : QObject(parent) {
    loadDefaults();
}

void MimeUtils::loadDefaults() {
    readAssociationFile();
}

void MimeUtils::readAssociationFile() noexcept {

    QFileInfo cache_finfo(MIME_CACHE_FILE_PATH);
    QFileInfo mime_finfo(MIME_FILE_NAME);

    if (cache_finfo.exists()) {
        if (mime_finfo.lastModified() > cache_finfo.lastModified()) {
            // NOTHING
        } else {

            QFile cache_file(MIME_CACHE_FILE_PATH);

            if (!cache_file.open(QIODevice::ReadOnly)) {
                qCritical() << "Error opening file for reading mime data from " << MIME_CACHE_FILE_PATH;
                return;
            }

            QDataStream in(&cache_file);
            in >> m_association_hash;
            cache_file.close();
            return;
        }
    }

    QHash<QString, QStringList> mimeAssociations;

    QFile file(MIME_FILE_NAME);

    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Error opening file:" << MIME_FILE_NAME;
        return; // Return empty hash if file can't be opened
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || !line.contains('=')) {
            continue;
        }

        QStringList parts = line.split('=');
        if (parts.size() != 2) {
            continue;
        }

        QString mimeType = parts[0].trimmed();
        QStringList applications = parts[1].split(';', Qt::SkipEmptyParts);

        mimeAssociations.insert(mimeType, applications);
    }

    QFile cache_file(MIME_CACHE_FILE_PATH);

    if (!cache_file.open(QIODevice::WriteOnly)) {
        qCritical() << "Unable to cache the mimetype data";
        return;
    }

    QDataStream out(&cache_file);
    out << mimeAssociations;

    file.close();
}

MimeUtils::~MimeUtils() {
    saveDefaults();
}

QString MimeUtils::getMimeType(const QString &path) {
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(path);
    //qDebug() << "mime type" << type.name() << path;
    return type.name();
}
//---------------------------------------------------------------------------

QStringList MimeUtils::getMimeTypes() const {
    return m_association_hash.keys();
}

void MimeUtils::open_in_app(const QString &file, const QString &app) noexcept {

}

void MimeUtils::open_files_in_app(const QStringList &files, const QString &app) noexcept {

}

QString MimeUtils::getAppForMimeType(const QString &mime) const
{
    return QString();
}

void MimeUtils::generateDefaults() {

    // Load list of applications
    QStringList names;

    // Find defaults; for each application...

    // Save defaults if changed
}

void MimeUtils::setDefault(const QString &mime, const QStringList &apps) {
    QString value = apps.join(";");
    m_association_hash.insert(mime, apps);
    m_defaultsChanged = true;
}

QStringList MimeUtils::getDefault(const QString &mime) const {
    if (m_association_hash.contains(mime))
        return m_association_hash[mime];
    else
        return QStringList();
}

void MimeUtils::saveDefaults() {
    m_defaultsChanged = false;
}
