#pragma once

#include "pch_thumbnailer.hpp"

static const QString THUMBNAIL_DIR = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QDir::separator() + "thumbnails" + QDir::separator() + "large";

class Thumbnailer : public QObject {

public:
    Thumbnailer();

    void generate_thumbnails(const QStringList &files) noexcept;
    void generate_thumbnail(const QString &file) noexcept;
    QImage get_image_from_cache(const QString &file_name) noexcept;

private:
    void create_thumbnails_dir() noexcept;
    QString file_uri_from_path(const QString &filePath) noexcept;
    void generate_thumbnail_for_image(const QString &filePath, const QString &path_uri) noexcept;
    void generate_thumbnail_for_pdf(const QString &filePath, const QString &path_uri) noexcept;
    void generate_thumbnail_for_video(const QString &filePath, const QString &path_uri) noexcept;

    QHash<QCryptographicHash, QImage> m_image_hash;
};
