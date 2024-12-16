#include "Thumbnailer.hpp"

Thumbnailer::Thumbnailer() {
    Magick::InitializeMagick(nullptr);
}

void Thumbnailer::generate_thumbnails(const QStringList &files) noexcept {
    QMimeDatabase mimeDB;
    for (const auto &file : files) {
      auto mimeType = mimeDB.mimeTypeForFile(file);
      QString mimeName = mimeType.name();
      QString URI = file_uri_from_path(file);
      if (mimeName.startsWith("image/")) {
          QFuture<void> future = QtConcurrent::run(&Thumbnailer::generate_thumbnail_for_image,
                                                   this, file, URI);
          qDebug() << file;
      } else if (mimeName == "application/pdf") {
          QFuture<void> future = QtConcurrent::run(&Thumbnailer::generate_thumbnail_for_pdf,
                                                   this, file, URI);
      } else if (mimeName.startsWith("video/")) {
          QFuture<void> future = QtConcurrent::run(&Thumbnailer::generate_thumbnail_for_video,
                                                   this, file, URI);
      }
    }
}

void Thumbnailer::generate_thumbnail(const QString &file) noexcept {
    QMimeDatabase mimeDB;
    auto mimeType = mimeDB.mimeTypeForFile(file);
    QString mimeName = mimeType.name();
    QString URI = file_uri_from_path(file);
    if (mimeName.startsWith("image/")) {
      QFuture<void> future = QtConcurrent::run(&Thumbnailer::generate_thumbnail_for_image,
                            this, file, URI);
    } else if (mimeName == "application/pdf") {
        QFuture<void> future = QtConcurrent::run(&Thumbnailer::generate_thumbnail_for_pdf,
                                                 this, file, URI);
    } else if (mimeName.startsWith("video/")) {
        QFuture<void> future = QtConcurrent::run(&Thumbnailer::generate_thumbnail_for_video,
                                                 this, file, URI);
    }
}

QString Thumbnailer::file_uri_from_path(const QString &path) noexcept {
    // Convert file path to URI
    QUrl fileUri = QUrl::fromLocalFile(path);
    QString uriString = fileUri.toString();

    // Hash the URI using MD5
    QByteArray hash = QCryptographicHash::hash(uriString.toUtf8(), QCryptographicHash::Md5);
    QString hashString = QString(hash.toHex());

    QString thumbnailPath = QDir(THUMBNAIL_DIR).filePath(hashString + ".png");

    return thumbnailPath;
}

QImage Thumbnailer::get_image_from_cache(const QString &file_name) noexcept {
    QString path_uri = file_uri_from_path(file_name);
    if (QFile::exists(path_uri))
        return QImage(path_uri);
    return QImage();
}

void Thumbnailer::generate_thumbnail_for_image(const QString &file_name, const QString &path_uri) noexcept {
    if (!QFile::exists(path_uri)) {
        //Do not preview if file size is greater than ‘max_preview_threshold’
        // QFileInfo file(file_name);
        // TODO
        // if (file.size() > m_max_preview_threshold) {
        //     return;
        // }

        try {
            Magick::Image image(file_name.toStdString());

            if (!image.isValid())
                return;

            image.resize(Magick::Geometry(256, 256));
            image.write(path_uri.toStdString());
        } catch (const Magick::Error &e) {
            qDebug() << e.what();
        }
    }
}

void Thumbnailer::generate_thumbnail_for_pdf(const QString &file_name,
                                             const QString &path_uri) noexcept {
    if (!QFile::exists(path_uri)) {
        // Handle PDF preview using Poppler
        std::unique_ptr<Poppler::Document> document = Poppler::Document::load(file_name);
        if (!document || document->isLocked()) {
            return;
        }

        std::unique_ptr<Poppler::Page> page = document->page(0);
        if (!page) {
            return;
        }

        QImage qimage = page->renderToImage(600, 600).scaled(256, 256, Qt::KeepAspectRatio,
                                                 Qt::SmoothTransformation);
        qimage.save(path_uri);
    }
}

void Thumbnailer::generate_thumbnail_for_video(const QString &file_name, const QString &path_uri) noexcept {
    // ffmpegthumbnailer command
    if (!QFile::exists(path_uri)) {
        QString command = "ffmpegthumbnailer";
        QStringList arguments;
        arguments << "-i" << file_name
        << "-o" << path_uri
        << "-s" << "256";  // Thumbnail size 128x128

        // Execute the command
        QProcess process;
        process.start(command, arguments);
        process.waitForFinished();

        if (process.exitCode() == 0) {
            qDebug() << "Thumbnail generated at:" << path_uri;
            return;
        } else {
            qDebug() << "Failed to generate thumbnail. Error:" << process.readAllStandardError();
            return;
        }
    }
}
