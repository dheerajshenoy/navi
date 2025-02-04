#pragma once

#include "pch/pch_previewpanel.hpp"

#include "FilePreviewWorker.hpp"
#include "TextEdit.hpp"
#include "ImageWidget.hpp"
#include "Thumbnailer.hpp"
#include "utils.hpp"

class PreviewPanel : public QWidget {
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
        m_img_widget->clear();
        QWidget::show();
    }

    inline void set_max_file_size_threshold(const qint64 &thresh) noexcept {
        m_preview_threshold = thresh;
    }

    inline std::string max_preview_threshold() noexcept {
        return utils::bytes_to_string(m_preview_threshold).toStdString();
    }

    inline void set_num_read_lines(const int &nlines) noexcept {
        m_num_read_lines = nlines;
    }

    inline int get_num_read_lines() noexcept {
        return m_num_read_lines;
    }


    // inline void SetSyntaxHighlighting(const bool &state) noexcept {
    //     m_syntax_highlighting_enabled = state;
    //     if (!state) {
    //         m_text_preview_widget->setSyntaxHighlighting(false);
    //     }
    // }

    inline void Set_preview_dimension(const int &width,
                                    const int &height) noexcept {
        m_img_widget->setFixedWidth(width);
        m_img_widget->setFixedHeight(height);
    }

    inline std::tuple<int, int> get_preview_dimension() noexcept {
        return std::tuple<int, int>(m_img_widget->width(), m_img_widget->height());
    }


    // inline void ToggleSyntaxHighlight() noexcept {
    //     m_syntax_highlighting_enabled = !m_syntax_highlighting_enabled;
    //     m_text_preview_widget->setSyntaxHighlighting(m_syntax_highlighting_enabled);
    // }

    void onFileSelected(const QString &filePath) noexcept;
    inline void clearImageCache() noexcept { m_image_cache.clear(); }

    Thumbnailer* thumbnailer() noexcept { return m_thumbnailer; }

    inline void rotateImage(const qreal &angle = 0.0f) noexcept {
        m_img_widget->rotate(angle);
    }

private:
    void previewArchive() noexcept;
    void loadImageAfterDelay() noexcept;
    QString readFirstFewLines(const QString &filePath, int lineCount = 5) noexcept;
    QString getMimeType( const QString& filePath ) {
        return QMimeDatabase().mimeTypeForFile( filePath ).name();
    }

    QVBoxLayout *m_layout = new QVBoxLayout();
    QStackedWidget *m_stack_widget = new QStackedWidget(this);
    ImageWidget *m_img_widget = new ImageWidget();
    TextEdit *m_text_preview_widget = new TextEdit();
    QWidget *m_empty_widget = new QWidget();

    QTimer *m_image_preview_timer = nullptr, *m_archive_preview_timer = nullptr,
           *m_text_file_preview_timer = nullptr;

    QString m_filepath;

    void showImagePreview(const QImage &image) noexcept;
    void showTextPreview() noexcept;
    void clearPreview() noexcept;
    bool m_syntax_highlighting_enabled = false;
    QCache<QString, QImage> m_image_cache;
    Thumbnailer *m_thumbnailer = new Thumbnailer();

    struct archive *m_archive;
    struct archive_entry *m_archive_entry;

    QStringList m_archive_mimetypes = {

      "application/zip",
      "application/x-rar-compressed",
      "application/x-gtar",
      "application/x-tar",
      "application/x-bzip2",
      "application/gzip",
      "application/x-7z-compressed",

    };

    qint64 m_preview_threshold = utils::parseFileSize("10M");
    int m_num_read_lines = 50;

};
