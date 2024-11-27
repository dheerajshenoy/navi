#pragma once

#include <QObject>
#include <QCryptographicHash>
#include <QHash>
#include <QUrl>
#include <QByteArray>
#include <qcryptographichash.h>
#include <QStandardPaths>
#include <QDir>
#include <QImage>
#include <QMimeDatabase>
#include <QMimeType>
#include <QMimeData>
#include <Magick++.h>
#include <poppler/qt6/poppler-qt6.h>
#include <QProcess>
#include <QtConcurrent/QtConcurrent>
#include <QPromise>
#include <QFuture>
#include <QThread>


static const QString THUMBNAIL_DIR = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QDir::separator() + "thumbnails" + QDir::separator() + "large";

class Thumbnailer : public QObject {

public:
    Thumbnailer();

    void generate_thumbnails(const QStringList &files) noexcept;
    QImage get_image_from_cache(const QString &file_name) noexcept;

private:

    QString file_uri_from_path(const QString &filePath) noexcept;
    void generate_thumbnail_for_image(const QString &filePath, const QString &path_uri) noexcept;
    void generate_thumbnail_for_pdf(const QString &filePath, const QString &path_uri) noexcept;
    void generate_thumbnail_for_video(const QString &filePath, const QString &path_uri) noexcept;

    QHash<QCryptographicHash, QImage> m_image_hash;
};