#pragma once

#include "pch/pch_thumbnailer.hpp"
#include "utils.hpp"

static const QString THUMBNAIL_DIR = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QDir::separator() + "thumbnails" + QDir::separator() + "large";

class Thumbnailer : public QObject {

public:
    Thumbnailer();

    void generate_thumbnails(const QStringList &files) noexcept;
    void generate_thumbnail(const QString &file) noexcept;
    QImage get_image_from_cache(const QString &file_name) noexcept;

    void set_max_thumbnail_threshold(const qint64 &bytes) noexcept { m_thumbnail_threshold = bytes; }
    qint64 get_max_thumbnail_threshold() noexcept { return m_thumbnail_threshold; }

private:
    void create_thumbnails_dir() noexcept;
    QString file_uri_from_path(const QString &filePath) noexcept;
    void generate_thumbnail_for_image(const QString &filePath, const QString &path_uri) noexcept;
    void generate_thumbnail_for_pdf(const QString &filePath, const QString &path_uri) noexcept;
    void generate_thumbnail_for_video(const QString &filePath, const QString &path_uri) noexcept;

    QHash<QCryptographicHash, QImage> m_image_hash;
    qint64 m_thumbnail_threshold = utils::parseFileSize("10M");
};
