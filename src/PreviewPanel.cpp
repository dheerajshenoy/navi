#include "PreviewPanel.hpp"

PreviewPanel::PreviewPanel(QWidget *parent) : QWidget(parent) {

    setLayout(m_layout);

    m_layout->addWidget(m_stack_widget, 0);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_stack_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    m_img_widget->setHidden(true);
    m_text_preview_widget->setHidden(true);

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

    m_stack_widget->addWidget(m_text_preview_widget);
    m_stack_widget->addWidget(m_img_widget);
    m_stack_widget->addWidget(m_empty_widget);
}

PreviewPanel::~PreviewPanel() {}

void PreviewPanel::showImagePreview(const QImage &image) noexcept {
    if (image.isNull())
        return;

    m_stack_widget->setCurrentIndex(1);
    m_img_widget->setImage(image.scaled(m_img_widget->width(), m_img_widget->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PreviewPanel::showTextPreview() noexcept {
    m_stack_widget->setCurrentIndex(0);
    auto linestrings = utils::readLinesFromFile(m_filepath, m_num_read_lines);

    if (linestrings.empty()) {
        m_text_preview_widget->clear();
    } else {
        m_text_preview_widget->setText(linestrings.join("\n"));
    }
}

void PreviewPanel::onFileSelected(const QString &filePath) noexcept {

    m_filepath = filePath;

    // Do not preview if it is a directory
    QFileInfo finfo(m_filepath);
    if (finfo.isDir()) {
        return;
    }

    if (finfo.exists() && (finfo.isSymLink() || finfo.isSymbolicLink()) && !finfo.symLinkTarget().isEmpty()) {
        m_filepath = QFile::symLinkTarget(filePath);
    }

    if (!utils::isBinaryFile(m_filepath.toStdString())) {
        m_text_file_preview_timer->start(150);
    } else {

        auto mimetype = getMimeType(m_filepath);

        if (m_archive_mimetypes.contains(mimetype)) {
            m_archive_preview_timer->start(150);
        }

        else if (mimetype == "application/pdf") {
            m_image_preview_timer->start(150);
        }

        else if (mimetype.startsWith("video/")) {
            m_image_preview_timer->start(150);
        }

        else if (mimetype.startsWith("image/") && !mimetype.contains("gif")) {
            m_image_preview_timer->start(150);
        }

        else {
            m_stack_widget->setCurrentIndex(2);
        }
    }
}

void PreviewPanel::loadImageAfterDelay() noexcept {
    QImage img;
    if (m_image_cache.contains(m_filepath)) {
        img = *m_image_cache[m_filepath];
    } else {
        img = m_thumbnailer->get_image_from_cache(m_filepath);
        m_image_cache.insert(m_filepath, new QImage(img));
    }

    showImagePreview(img);
}

void PreviewPanel::clearPreview() noexcept { m_img_widget->clear(); }

void PreviewPanel::previewArchive() noexcept {
    m_archive = archive_read_new();
    archive_read_support_filter_all(m_archive);
    archive_read_support_format_all(m_archive);
    auto r = archive_read_open_filename(m_archive, m_filepath.toUtf8().constData(), 65536); // Block size = 64kb

    if (r != ARCHIVE_OK) {
        qWarning() << "Failed to open archive:" << archive_error_string(m_archive);
        archive_read_free(m_archive);
        return;
    }

    int count = 0;

    QStringList fileNames;

    while (archive_read_next_header(m_archive, &m_archive_entry) == ARCHIVE_OK) {
        fileNames.append(archive_entry_pathname(m_archive_entry));
        count++;
    }

    archive_read_close(m_archive);
    archive_read_free(m_archive);
    m_text_preview_widget->clear();

    if (count == 0) {
        fileNames.append(QString("File: %1 (items: %2)").arg(m_filepath).arg(count));
    } else if (count > 0) {
        fileNames.append(QString("\n\nFile: %1 (items: %2)").arg(m_filepath).arg(count));
    }

    m_text_preview_widget->setPlainText(fileNames.join("\n"));

    m_stack_widget->setCurrentIndex(0);
}
