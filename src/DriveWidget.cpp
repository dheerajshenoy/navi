#include "DriveWidget.hpp"

DriveWidget::DriveWidget(QWidget *parent) : QDialog(parent) {
    m_layout->addWidget(m_drives_table_widget);

    setModal(false);
    QStorageInfo storageInfo;
    auto mountedVolumes = QStorageInfo::mountedVolumes();

    // m_drives_table_widget->horizontalHeader()->setStretchLastSection(true);

    m_drives_table_widget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    // m_drives_table_widget->setRowCount(mountedVolumes.size());

    auto header = m_drives_table_widget->horizontalHeader();
    m_drives_table_widget->setColumnCount(4);
    m_drives_table_widget->setHorizontalHeaderLabels({"Drive", "FS Type", "Size", "Mount Point"});
    header->setStretchLastSection(true);
    m_drives_table_widget->setEditTriggers(QTableWidget::EditTrigger::NoEditTriggers);
    header->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    this->setLayout(m_layout);

    connect(m_drives_watcher, &QFileSystemWatcher::directoryChanged, this,
            [&](const QString &path) { loadDrives(); });

    loadDrives();
    m_drives_watcher->addPath("/dev/");
    connect(m_drives_table_widget, &QTableWidget::cellDoubleClicked, this, &DriveWidget::cellDoubleClicked);
}

void DriveWidget::loadDrives() noexcept {

    uint i = 0;
    m_drives_table_widget->setRowCount(0);
    auto driveList = utils::getDrives();

    for (const auto &drive : driveList) {

        auto name = new QTableWidgetItem(drive.name);
        auto fstype = new QTableWidgetItem(drive.fstype);
        auto size = new QTableWidgetItem(drive.size);
        auto mountPoint = new QTableWidgetItem(drive.mountPoint);
        m_drives_table_widget->insertRow(i);
        m_drives_table_widget->setItem(i, 0, name);
        m_drives_table_widget->setItem(i, 1, fstype);
        m_drives_table_widget->setItem(i, 2, size);
        m_drives_table_widget->setItem(i, 3, mountPoint);
        i++;
    }
}

void DriveWidget::cellDoubleClicked(int row, int col) noexcept {
    auto mountPoint = m_drives_table_widget->item(row, 3)->text();
    auto driveName = m_drives_table_widget->item(row, 0)->text();
    driveName = QString("/dev") + QDir::separator() + driveName;
    if (mountPoint.isEmpty() || mountPoint.isNull()) {
        emit driveMountRequested(driveName);
    } else
        emit driveLoadRequested(mountPoint);
}