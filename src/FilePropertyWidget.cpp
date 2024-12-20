#include "FilePropertyWidget.hpp"

FilePropertyWidget::FilePropertyWidget(const QString &itemPath, QWidget *parent)
: QDialog(parent) {
    m_layout = new QFormLayout();
    this->setLayout(m_layout);

    QFileInfo fileInfo(itemPath);
    if (!fileInfo.exists())
        return;

    QLabel *m_files_count_label = new QLabel();
    QLabel *m_total_size_label = new QLabel();
    QLabel *iconLabel = new QLabel;
    QString filePath = fileInfo.filePath();
    QIcon icon = QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(filePath).iconName());
    QPixmap iconPixmap = icon.pixmap(QSize(32, 32));
    iconLabel->setPixmap(iconPixmap);

    if (fileInfo.isFile()) {
        m_layout->addRow("Name", new QLabel(fileInfo.fileName()));
        m_layout->addRow("Type", new QLabel("File"));
        m_layout->addRow(
                         "Size", new QLabel(m_locale.formattedDataSize(fileInfo.size())));
        m_layout->addRow("Last modified", new QLabel(fileInfo.lastModified().toString()));
        m_layout->addRow("Path", new QLabel(filePath));
        m_layout->addRow("Permissions",
                         new QLabel(utils::getPermString(fileInfo)));
        m_layout->addRow("Mime Type", new QLabel(QMimeDatabase().mimeTypeForFile(filePath).name()));
    }
    else if (fileInfo.isDir()) {
        m_layout->addRow("Name", new QLabel(fileInfo.fileName()));
        m_layout->addRow("Type", new QLabel("Directory"));
        m_layout->addRow("Size", m_total_size_label);
        m_layout->addRow("Total count of files", m_files_count_label);
        m_layout->addRow("Last modified", new QLabel(fileInfo.lastModified().toString()));
        m_layout->addRow("Path", new QLabel(filePath));
        m_layout->addRow("Permissions", new QLabel(utils::getPermString(fileInfo)));
        m_layout->addRow("Mime Type", new QLabel(QMimeDatabase().mimeTypeForFile(filePath).name()));
    }

    m_layout->addRow("Icon", iconLabel);
    QPushButton *closeButton = new QPushButton("Close");
    m_layout->addRow(closeButton);
    connect(closeButton, &QPushButton::clicked, this, [&]() { this->close(); });

    future = QtConcurrent::run(&utils::getFolderInfo, itemPath);
    connect(future_watcher, &QFutureWatcher<utils::FolderInfo>::finished, this,
            [this, m_files_count_label, m_total_size_label]() {
                utils::FolderInfo finfo = future_watcher->result();
                m_files_count_label->setText(QString::number(finfo.count));
                m_total_size_label->setText(m_locale.formattedDataSize(finfo.size));
            });
    future_watcher->setFuture(future);
}

FilePropertyWidget::~FilePropertyWidget() {
    if (future_watcher->isRunning()) {
        future_watcher->cancel();             // Request cancellation
        future_watcher->waitForFinished();   // Ensure task completion
    }
    future_watcher->deleteLater();           // Clean up
}