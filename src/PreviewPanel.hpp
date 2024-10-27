#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMimeDatabase>
#include <QFile>
#include <QTextEdit>
#include <QtConcurrent/QtConcurrent>
#include <QPromise>
#include <QFutureWatcher>
#include <QStackedWidget>

#include "FilePreviewWorker.hpp"
#include "TreeSitterTextEdit.hpp"

class PreviewPanel : public QStackedWidget {
    Q_OBJECT

signals:
void visibilityChanged(const bool& state);

public:
    PreviewPanel(QWidget *parent = nullptr);
    ~PreviewPanel();

    void loadImage(const QString& filepath);
    void hide() noexcept {
        emit visibilityChanged(false);
        QWidget::hide();
    }

    void show() noexcept {
        emit visibilityChanged(true);
        m_img_preview_widget->clear();
        QWidget::show();
    }

    void onFileSelected(const QString &filePath) noexcept;

private:
    QString readFirstFewLines(const QString &filePath, int lineCount = 5) noexcept;
    QString getMimeType( const QString& filePath ) {
        return QMimeDatabase().mimeTypeForFile( filePath ).name();
    }

    QLabel *m_img_preview_widget = new QLabel();
    TreeSitterTextEdit *m_text_preview_widget = new TreeSitterTextEdit();
    QWidget *m_empty_widget = new QWidget();

    FilePreviewWorker *m_worker;
    QThread *m_workerThread;

    void showImagePreview(const QPixmap &pixmap) noexcept;
    void showTextPreview(const QString &pixmap) noexcept;
    void cancelPreview() noexcept;
};