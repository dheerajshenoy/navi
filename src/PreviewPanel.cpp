#include "PreviewPanel.hpp"
#include "SyntaxHighlighterTS.hpp"

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
    m_image_preview_timer->setSingleShot(true);

    connect(m_image_preview_timer, &QTimer::timeout, this, &PreviewPanel::loadImageAfterDelay);

    this->addWidget(m_text_preview_widget);
    this->addWidget(m_img_widget);
    this->addWidget(m_empty_widget);
}

PreviewPanel::~PreviewPanel() {}

void PreviewPanel::showImagePreview(const QImage &image) noexcept {
    this->setCurrentIndex(1);
    m_img_widget->setImage(image);
}

void PreviewPanel::showTextPreview(const QString &preview,
                                   const SyntaxHighlighterTS::Language &language) noexcept {
    this->setCurrentIndex(0);
    if (m_syntax_highlighting_enabled)
        m_text_preview_widget->setLanguage(language);
    m_text_preview_widget->setText(preview);
}

void PreviewPanel::onFileSelected(const QString &filePath) noexcept {
    m_img_widget->clear();
    m_image_filepath = filePath;
    m_image_preview_timer->start(150);
}

void PreviewPanel::loadImageAfterDelay() noexcept {
    if (m_image_cache_hash.contains(m_image_filepath)) {
        QImage img = m_image_cache_hash[m_image_filepath];
        showImagePreview(img);
        return;
    } else {
        QImage img = m_thumbnailer->get_image_from_cache(m_image_filepath);
        if (!img.isNull()) {
            m_image_cache_hash.insert(m_image_filepath, img);
            showImagePreview(img);
        } else {
            m_thumbnailer->generate_thumbnail(m_image_filepath);
        }
    }
}

void PreviewPanel::clearPreview() noexcept { m_img_widget->clear(); }