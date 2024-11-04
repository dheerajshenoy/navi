#include "FilePropertyWidget.hpp"

FilePropertyWidget::FilePropertyWidget(const QString &itemPath, QWidget *parent)
: QDialog(parent) {
    m_layout = new QFormLayout();
    this->setLayout(m_layout);

    QFileInfo fileInfo(itemPath);
    if (!fileInfo.exists())
        return;

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
        m_layout->addRow("Size",
                         new QLabel(m_locale.formattedDataSize(fileInfo.size())));
        m_layout->addRow("Last modified", new QLabel(fileInfo.lastModified().toString()));
        m_layout->addRow("Path", new QLabel(filePath));
        m_layout->addRow("Permissions", new QLabel(utils::getPermString(fileInfo)));
        m_layout->addRow("Mime Type", new QLabel(QMimeDatabase().mimeTypeForFile(filePath).name()));
    }

    m_layout->addRow("Icon", iconLabel);
    QPushButton *closeButton = new QPushButton("Close");
    m_layout->addRow(closeButton);
    connect(closeButton, &QPushButton::clicked, this, [&]() { this->close(); });
}