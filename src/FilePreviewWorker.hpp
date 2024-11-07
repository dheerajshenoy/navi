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
#include <poppler/qt6/poppler-qt6.h>
#include <QPainter>
#include <qnamespace.h>

class FilePreviewWorker : public QObject {
    Q_OBJECT

    public:
    explicit FilePreviewWorker(QObject *parent = nullptr) : QObject(parent) {}

    void setMaxPreviewThreshold(const qint64 &maxThreshold) noexcept {
        m_max_preview_threshold = maxThreshold;
    }

public slots:
    void loadPreview(const QString &filePath) {

        if (isCancelled) return; // Check for cancellation

        QMimeDatabase mimeDatabase;
        QMimeType mimeType =
            mimeDatabase.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

        QString mimeName = mimeType.name();

        if (mimeName.startsWith("image/")) {

            // Do not preview if file size is greater than ‘max_preview_threshold’
            QFileInfo file(filePath);
            if (file.size() > m_max_preview_threshold) {
                emit clearPreview();
                return;
            }

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
            // Do not preview if file size is greater than ‘max_preview_threshold’
            QFileInfo _file(filePath);
            if (_file.size() > m_max_preview_threshold) {
                emit clearPreview();
                return;
            }
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
        } else if (mimeName == "application/pdf") {

            // Handle PDF preview using MuPDF
            if (isCancelled)
                return; // Check for cancellation again

            std::unique_ptr<Poppler::Document> document = Poppler::Document::load(filePath);
            if (!document || document->isLocked()) {
                emit errorOccurred("Could not open PDF");
                return;
            }

            std::unique_ptr<Poppler::Page> page = document->page(0);
            if (!page) {
                emit errorOccurred("Could not load first page of the PDF");
                return;
            }

            QImage image = page->renderToImage(300, 300);
            emit imagePreviewReady(QPixmap::fromImage(image.copy()));
        }
        else
            emit errorOccurred("Unsupported file type for preview.");
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
    void clearPreview();

private:
    bool isCancelled = false;
    QMutex mutex;

    qint64 m_max_preview_threshold = 1e+7;
};
