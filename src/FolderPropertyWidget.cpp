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
// new QLabel(m_locale.formattedDataSize(getFolderSize(folderPath))));

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

    QLocale locale;
    QFutureWatcher<FolderInfo> *future_watcher = new QFutureWatcher<FolderInfo>(this);
    QFuture<FolderInfo> future = QtConcurrent::run(&FolderPropertyWidget::getFolderInfo, this, folderPath);
    connect(future_watcher, &QFutureWatcher<FolderInfo>::finished, this,
            [future_watcher, item_count_label, size_label, locale]() {
        FolderInfo finfo = future_watcher->result();
        item_count_label->setText(QString::number(finfo.count));
        size_label->setText(locale.formattedDataSize(finfo.size));
    });
    future_watcher->setFuture(future);
    this->show();

    connect(closeButton, &QPushButton::clicked, this, [this, future_watcher]() {
        if (future_watcher->isRunning()) {
            future_watcher->cancel();             // Request cancellation
            future_watcher->waitForFinished();   // Ensure task completion
        }
        future_watcher->deleteLater();           // Clean up
        this->close();  // Close the current window
    });

}

FolderPropertyWidget::FolderInfo FolderPropertyWidget::getFolderInfo(const QString &folderPath) noexcept {
    uint32_t count = 0;

    quint64 size = 0;
    QDir dir(folderPath);
    //calculate total size of current directories' files
    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size += fi.size();
        count++;
    }

    //add size of child directories recursively
    QDir::Filters dirFilters =
        QDir::Dirs | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden;

    for(QString childDirPath : dir.entryList(dirFilters)) {
        auto finfo = getFolderInfo(folderPath + QDir::separator() + childDirPath);
        size += finfo.size;
        count += finfo.count;
    }

    FolderInfo finfo;
    finfo.count = count;
    finfo.size = size;

    return finfo;
}