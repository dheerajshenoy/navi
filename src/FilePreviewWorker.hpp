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
#include "SyntaxHighlighter.hpp"

class FilePreviewWorker : public QObject {
    Q_OBJECT

public:
      explicit FilePreviewWorker(QObject *parent = nullptr) : QObject(parent) {
        Magick::InitializeMagick(nullptr);
}

void setMaxPreviewThreshold(const qint64 &maxThreshold) noexcept {
    m_max_preview_threshold = maxThreshold;
}

public slots:
void loadPreview(const QString &filePath) {

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

        Magick::Image image;
        image.read(filePath.toStdString());

        if (!image.isValid())
            return;

        image.thumbnail(Magick::Geometry(200, 200));

        int width = image.columns();
        int height = image.rows();

        // Ensure the image is in RGB format for compatibility with QImage
        Magick::Blob blob;
        image.write(&blob, "RGBA");  // Convert to RGBA

        const uchar *data = reinterpret_cast<const uchar *>(blob.data());
        QImage qimage(data, width, height, QImage::Format_RGBA8888);
        // qimage.setDotsPerMeterX(300);
        // qimage.setDotsPerMeterY(300);
        emit imagePreviewReady(qimage.copy());
    } else if (mimeName.startsWith("text/") ||
               mimeName == "application/json") {
        // Do not preview if file size is greater than ‘max_preview_threshold’
        QFileInfo _file(filePath);
        if (_file.size() > m_max_preview_threshold) {
            emit clearPreview();
            return;
        }
        // detect language
        SyntaxHighlighter::Language language;
        QString lang = filePath.mid(filePath.lastIndexOf("."));
        qDebug() << lang;

        if (lang == ".py" || lang == ".pyx" || lang == ".pyc")
            language = SyntaxHighlighter::Language::PYTHON;
        else if (lang == ".cpp" || lang == ".hpp")
            language = SyntaxHighlighter::Language::CPP;
        else if (lang == ".c" || lang == ".h")
            language = SyntaxHighlighter::Language::C;

        // Load text preview
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            emit textPreviewReady(content, language);
        } else {
            emit errorOccurred("Failed to load text preview.");
        }
    } else if (mimeName == "application/pdf") {
        // Handle PDF preview using Poppler
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

        QImage qimage = page->renderToImage(600, 600)
                       .scaled(400,
                               400,
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
        emit imagePreviewReady(qimage.copy());
    }
    else
        emit errorOccurred("Unsupported file type for preview.");
}

signals:
    void imagePreviewReady(const QImage &pix);
    void errorOccurred(const QString &errorMessage);
    void textPreviewReady(const QString &textContent,
                        const SyntaxHighlighter::Language &language);
    void clearPreview();

private:
    qint64 m_max_preview_threshold = 1e+7;
};
