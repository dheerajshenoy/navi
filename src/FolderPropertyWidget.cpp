#include "FolderPropertyWidget.hpp"
#include <qdiriterator.h>

FolderPropertyWidget::FolderPropertyWidget(const QString &folderPath, QWidget *parent)
: QDialog(parent) {
    m_layout = new QFormLayout();
    this->setLayout(m_layout);

    QFileInfo fileInfo(folderPath);
    if (!fileInfo.exists())
        return;

    QLabel *iconLabel = new QLabel;
    QString filePath = fileInfo.filePath();
    QIcon icon = QIcon::fromTheme(QMimeDatabase().mimeTypeForFile(filePath).iconName());
    QPixmap iconPixmap = icon.pixmap(QSize(32, 32));
    iconLabel->setPixmap(iconPixmap);

    QLabel *size_label = new QLabel("Calculating...");
    QLabel *item_count_label = new QLabel("Calculating...");

    m_layout->addRow("Name", new QLabel(fileInfo.fileName()));
    m_layout->addRow("Location", new QLabel(filePath));
    m_layout->addRow("Total files count", item_count_label);
    m_layout->addRow("Size", size_label);
    m_layout->addRow("Size on Disk", new QLabel("TODO"));
    m_layout->addRow("Last modified", new QLabel(fileInfo.lastModified().toString()));
    m_layout->addRow("Permissions", new QLabel(utils::getPermString(fileInfo)));

    m_layout->addRow("Icon", iconLabel);
    QPushButton *closeButton = new QPushButton("Close");
    m_layout->addRow(closeButton);

    future = QtConcurrent::run(&utils::getFolderInfo, folderPath);
    connect(future_watcher, &QFutureWatcher<utils::FolderInfo>::finished, this,
            [this, item_count_label, size_label]() {
                utils::FolderInfo finfo = future_watcher->result();
                item_count_label->setText(QString::number(finfo.count));
                size_label->setText(locale.formattedDataSize(finfo.size));
            });
    future_watcher->setFuture(future);
    this->show();

    connect(closeButton, &QPushButton::clicked, this, [this] () {
        this->close();
    });

}

FolderPropertyWidget::~FolderPropertyWidget() {
    if (future_watcher->isRunning()) {
        future_watcher->cancel();             // Request cancellation
        future_watcher->waitForFinished();   // Ensure task completion
    }
    future_watcher->deleteLater();           // Clean up
}