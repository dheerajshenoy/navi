#include "PreviewPanel.hpp"

PreviewPanel::PreviewPanel(QWidget *parent) : QStackedWidget(parent) {

    // this->setContentsMargins(0, 0, 0, 0);

    m_img_preview_widget->setHidden(true);
    m_text_preview_widget->setHidden(true);


    m_img_preview_widget->setAlignment(Qt::AlignmentFlag::AlignTop);
    m_img_preview_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_text_preview_widget->setAcceptRichText(true);
    m_text_preview_widget->setReadOnly(true);
    // m_text_preview_widget->setFrameShadow(QTextEdit::Shadow::Plain);
    // m_text_preview_widget->setFrameStyle(0);

    m_worker = new FilePreviewWorker();
    m_workerThread = new QThread(this);
    m_worker->moveToThread(m_workerThread);

    connect(m_worker, &FilePreviewWorker::clearPreview, this, &PreviewPanel::clearPreview);
    connect(m_worker, &FilePreviewWorker::imagePreviewReady, this, &PreviewPanel::showImagePreview);
    connect(m_worker, &FilePreviewWorker::textPreviewReady, this, &PreviewPanel::showTextPreview);
    connect(m_worker, &FilePreviewWorker::errorOccurred, this, [&]() {
        this->setCurrentIndex(2);
    });

    this->addWidget(m_text_preview_widget);
    this->addWidget(m_img_preview_widget);
    this->addWidget(m_empty_widget);

    m_workerThread->start();
}

PreviewPanel::~PreviewPanel() {
    cancelPreview();
    m_workerThread->quit();
    m_workerThread->wait();  // Ensure the worker thread stops before cleanup
    delete m_worker;
}

void PreviewPanel::showImagePreview(const QPixmap &preview) noexcept {
    this->setCurrentIndex(1);
    m_img_preview_widget->setPixmap(preview.scaled(400, 400, Qt::KeepAspectRatio));
}

void PreviewPanel::showTextPreview(const QString &preview) noexcept {
    this->setCurrentIndex(0);
    m_text_preview_widget->setText(preview);
}

void PreviewPanel::onFileSelected(const QString &filePath) noexcept {
    cancelPreview();  // Cancel any ongoing preview
    m_worker->reset(); // Reset the worker's cancellation state
    QMetaObject::invokeMethod(m_worker, "loadPreview", Q_ARG(QString, filePath));
}

void PreviewPanel::cancelPreview() noexcept {
    m_worker->cancel();  // Signal cancellation to the worker
}

void PreviewPanel::clearPreview() noexcept {
    m_img_preview_widget->clear();
}