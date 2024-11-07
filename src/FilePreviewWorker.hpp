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
#include <mupdf/fitz.h>
#include <mupdf/fitz/geometry.h>
#include <QPainter>

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

        // Do not preview if file size is greater than ‘max_preview_threshold’
        QFileInfo file(filePath);
        if (file.size() > m_max_preview_threshold) {
            emit clearPreview();
            return;
        }

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
        } else if (mimeName == "application/pdf") {

        // Handle PDF preview using MuPDF
        if (isCancelled) return; // Check for cancellation again

        fz_context *context = fz_new_context(nullptr, nullptr, FZ_STORE_DEFAULT);
        if (!context) {
            emit errorOccurred("Failed to initialize MuPDF context.");
            return;
        }
        /* Register the default file types to handle. */
        fz_try(context)
        fz_register_document_handlers(context);
        fz_catch(context)
        {
            emit errorOccurred("Failed to initialize MuPDF context.");
            return;
        }

        fz_document *doc = fz_open_document(context, filePath.toUtf8().constData());
        int pageCount = fz_count_pages(context, doc);
        if (pageCount <= 0) {
            fz_drop_document(context, doc);
            fz_drop_context(context);
            emit errorOccurred("No pages in PDF document.");
            return;
        }

        // Render the first page
        fz_page *page = fz_load_page(context, doc, 0);  // Load first page (0-based index)
        if (!page) {
            fz_drop_document(context, doc);
            fz_drop_context(context);
            emit errorOccurred("Failed to load first page.");
            return;
        }

        fz_matrix transform = fz_scale(1.0f, 1.0f);
        fz_pixmap *pix = fz_new_pixmap_from_page_number(
                                                        context, doc, 0, transform, fz_device_rgb(context), 0);


        // Create QImage
        QImage qImage(pix->samples, pix->w, pix->h, pix->stride, QImage::Format_RGB888);

        // Render page to QImage
        fz_device *device = fz_new_draw_device(context, transform, pix);
        fz_run_page(context, page, device, transform, nullptr);
        fz_drop_device(context, device);

        // Send image preview
        emit imagePreviewReady(QPixmap::fromImage(qImage.copy()));

        // Clean up
        fz_drop_page(context, page);
        fz_drop_document(context, doc);
        fz_drop_context(context);

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
    void clearPreview();

private:
    bool isCancelled = false;
    QMutex mutex;

    qint64 m_max_preview_threshold = 1e+7;
};
