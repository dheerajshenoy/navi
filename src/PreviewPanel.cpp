#include "PreviewPanel.hpp"
#include <archive.h>

PreviewPanel::PreviewPanel(QWidget *parent) : QStackedWidget(parent) {

    // this->setContentsMargins(0, 0, 0, 0);

    m_img_widget->setHidden(true);
    m_text_preview_widget->setHidden(true);

    // m_img_widget->setAlignment(Qt::AlignmentFlag::AlignTop);
    m_img_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_text_preview_widget->setAcceptRichText(true);
    m_text_preview_widget->setReadOnly(true);
    // m_text_preview_widget->setFrameShadow(QTextEdit::Shadow::Plain);
    // m_text_preview_widget->setFrameStyle(0);

    m_image_preview_timer = new QTimer(this);
    m_archive_preview_timer = new QTimer(this);
    m_text_file_preview_timer = new QTimer(this);

    m_image_preview_timer->setSingleShot(true);
    m_archive_preview_timer->setSingleShot(true);
    m_text_file_preview_timer->setSingleShot(true);

    connect(m_image_preview_timer, &QTimer::timeout, this,
            &PreviewPanel::loadImageAfterDelay);

    connect(m_archive_preview_timer, &QTimer::timeout, this,
            &PreviewPanel::previewArchive);

    connect(m_text_file_preview_timer, &QTimer::timeout, this, &PreviewPanel::showTextPreview);

    this->addWidget(m_text_preview_widget);
    this->addWidget(m_img_widget);
    this->addWidget(m_empty_widget);
}

PreviewPanel::~PreviewPanel() {}

void PreviewPanel::showImagePreview(const QImage &image) noexcept {
    this->setCurrentIndex(1);
    m_img_widget->setImage(image);
}

void PreviewPanel::showTextPreview() noexcept {
    this->setCurrentIndex(0);
    auto linestrings = utils::readLinesFromFile(m_filepath, m_num_read_lines);
    m_text_preview_widget->setText(linestrings.join("\n"));
}

void PreviewPanel::onFileSelected(const QString &filePath) noexcept {
    m_img_widget->clear();
    auto mimetype = getMimeType(filePath);
    m_filepath = filePath;
    if (mimetype.startsWith("text")) {
        m_text_file_preview_timer->start(150);
    } else if (m_archive_mimetypes.contains(mimetype)) {
        m_archive_preview_timer->start(150);
    } else
        m_image_preview_timer->start(150);
}

void PreviewPanel::loadImageAfterDelay() noexcept {
    if (m_image_cache_hash.contains(m_filepath)) {
        QImage img = m_image_cache_hash[m_filepath];
        showImagePreview(img);
    } else {
        QImage img = m_thumbnailer->get_image_from_cache(m_filepath);
        if (!img.isNull()) {
            m_image_cache_hash.insert(m_filepath, img);
            showImagePreview(img);
        }
    }
}

void PreviewPanel::clearPreview() noexcept { m_img_widget->clear(); }

void PreviewPanel::previewArchive() noexcept {
    m_archive = archive_read_new();
    archive_read_support_filter_all(m_archive);
    archive_read_support_format_all(m_archive);
    auto r = archive_read_open_filename(m_archive, m_filepath.toUtf8().constData(),
                                   10240); // Block size = 10KB

    if (r != ARCHIVE_OK) {
        qWarning() << "Failed to open archive:" << archive_error_string(m_archive);
        archive_read_free(m_archive);
        return;
    }

    int count = 0;

    m_text_preview_widget->clear();

    while (archive_read_next_header(m_archive, &m_archive_entry) == ARCHIVE_OK) {
        QString fileName = archive_entry_pathname(m_archive_entry);
        m_text_preview_widget->append(fileName);
        count++;
    }

    m_text_preview_widget->append("");
    m_text_preview_widget->append("");
    m_text_preview_widget->append(QString("File: %1 (items: %2)")
                           .arg(m_filepath)
                           .arg(count));

    archive_read_close(m_archive);
    archive_read_free(m_archive);

    this->setCurrentIndex(0);
}
