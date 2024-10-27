#pragma once

#include <QObject>
#include <QPixmap>
#include <QMutex>
#include <QMutexLocker>
#include <QMimeDatabase>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <Magick++.h>

class FilePreviewWorker : public QObject {
    Q_OBJECT

    public:
    explicit FilePreviewWorker(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    // void loadPreview(const QString &filePath) {
    //     QPixmap preview;

    //     // Check for cancellation before starting the task
    //     if (isCancelled) return;

    //     if (preview.load(filePath)) {
    //         // Check for cancellation again before emitting
    //         if (isCancelled) return;
    //         emit previewReady(preview);
    //     } else {
    //         emit errorOccurred("Failed to load preview.");
    //     }
    // }

    // void loadPreview(const QString &filePath) {
    //     // Check for cancellation before starting the task
    //     if (isCancelled) return;

    //     QMimeDatabase mimeDatabase;
    //     QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

  //     if (mimeType.name().startsWith("image/")) {
  //         // Load image preview
  //         QPixmap preview;
  //         if (preview.load(filePath)) {
  //             if (isCancelled) return; // Check cancellation again
  //             emit imagePreviewReady(preview);
  //         } else {
  //             emit errorOccurred("Failed to load image preview.");
  //         }
  //     } else if (mimeType.name().startsWith("text/")) {
  //         // Load text preview
  //         QFile file(filePath);
  //         if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
  //             if (isCancelled) return; // Check cancellation again
  //             QTextStream in(&file);
  //             QString content = in.readAll();
  //             emit textPreviewReady(content);
  //         } else {
  //             emit errorOccurred("Failed to load text preview.");
  //         }
  //     } else {
  //         emit errorOccurred("Unsupported file type for preview.");
  //     }
    // }

    void loadPreview(const QString &filePath) {
        if (isCancelled) return; // Check for cancellation

        QMimeDatabase mimeDatabase;
        QMimeType mimeType =
            mimeDatabase.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

        QString mimeName = mimeType.name();

        if (mimeName.startsWith("image/")) {

            Magick::InitializeMagick(nullptr);
            Magick::Image image;
            image.read(filePath.toStdString());

            if (!image.isValid())
                return;

            int width = image.columns(), height = image.rows();

            // Ensure the image is in RGB format for compatibility with QImage
            Magick::Blob blob;
            image.write(&blob, "RGBA");  // Convert to RGBA

            // Create a QImage using the raw data from the Magick::Image
            const uchar *data = reinterpret_cast<const uchar *>(blob.data());
            QImage qimage(data, width, height, QImage::Format_RGBA8888);

            emit imagePreviewReady(QPixmap::fromImage(qimage.copy()));
            // Use ImageMagick to load the image
        } else if (mimeName.startsWith("text/")) {
            // Load text preview
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                if (isCancelled) return; // Check cancellation again
                QTextStream in(&file);
                QString content = in.readAll();
                emit textPreviewReady(content);
            } else {
                emit errorOccurred("Failed to load text preview.");
            }
        } else {
            emit errorOccurred("Unsupported file type for preview.");
        }
    }

    void cancel() {
        QMutexLocker locker(&mutex);
        isCancelled = true;
    }

    void reset() {
        QMutexLocker locker(&mutex);
        isCancelled = false;
    }

    signals:
    void imagePreviewReady(const QPixmap &preview);
    void errorOccurred(const QString &errorMessage);
    void textPreviewReady(const QString &textContent);

private:
    bool isCancelled = false;
    QMutex mutex;
};
