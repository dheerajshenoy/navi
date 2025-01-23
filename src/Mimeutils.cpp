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
    return type.name();
}
//---------------------------------------------------------------------------

QStringList MimeUtils::getMimeTypes() const {
    return m_association_hash.keys();
}

void MimeUtils::open_file_in_app(const QString &file) noexcept {
    auto exec = m_desktopFile.getExec();
    auto split = exec.split(" ", Qt::SkipEmptyParts);
    auto cmd = split.mid(0).first();
    auto args = split.mid(0).last().split(" ", Qt::SkipEmptyParts);

    args.replaceInStrings("%F", file);

    QProcess process;
    process.startDetached(cmd, args);
}

void MimeUtils::open_files_in_app(const QStringList &files) noexcept {
    auto exec = m_desktopFile.getExec();
    auto split = exec.split(" ", Qt::SkipEmptyParts);
    auto cmd = split.mid(0).first();
    auto args = split.mid(0).last().split(" ", Qt::SkipEmptyParts);

    args.replaceInStrings("%F", files.join(" "));

    QProcess process;
    process.startDetached(cmd, args);
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

QStringList MimeUtils::apps_for_file(const QString &path) noexcept {
    auto mime = getMimeType(path);
    return m_association_hash[mime];
}

QList<QAction*> MimeUtils::app_actions_for_files(const QStringList &paths) noexcept {
    QList<QAction*> actions_list;

    QStringList apps = apps_for_file(paths.at(0));

    if (apps.isEmpty())
        return {};

    actions_list.reserve(apps.size());
    m_desktopFile = DesktopFile(apps.at(0));

    for (const auto &app : apps) {
        QAction *action;
        if (m_desktopFile.getIcon().isEmpty()) {
            action = new QAction(m_desktopFile.getName(), this);
        } else {
            action = new QAction(QIcon::fromTheme(m_desktopFile.getIcon()),
                                 m_desktopFile.getName(),
                                 this);
        }

        connect(action, &QAction::triggered, this, [&, paths]() {
            open_files_in_app(paths);
        });

        actions_list.append(action);
    }

    return actions_list;
}


QList<QAction*> MimeUtils::app_actions_for_file(const QString &path) noexcept {
    QList<QAction*> actions_list;

    QStringList apps = apps_for_file(path);

    if (apps.isEmpty())
        return {};

    actions_list.reserve(apps.size());
    m_desktopFile = DesktopFile(apps.at(0));

    for (const auto &app : apps) {
        QAction *action;
        if (m_desktopFile.getIcon().isEmpty()) {
            action = new QAction(m_desktopFile.getName(), this);
        } else {
            action = new QAction(QIcon::fromTheme(m_desktopFile.getIcon()),
                                 m_desktopFile.getName(),
                                 this);
        }

        connect(action, &QAction::triggered, this, [&, path]() {
            open_file_in_app(path);
        });

        actions_list.append(action);
    }

    return actions_list;
}
