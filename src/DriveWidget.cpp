#include "DriveWidget.hpp"

DriveWidget::DriveWidget() : KDDockWidgets::QtWidgets::DockWidget("Drives") {
    QWidget *w = new QWidget(this);
    w->setLayout(m_layout);
    this->setWidget(w);
    m_layout->addWidget(m_drives_table_widget);

    QStorageInfo storageInfo;
    auto mountedVolumes = QStorageInfo::mountedVolumes();

    // m_drives_table_widget->horizontalHeader()->setStretchLastSection(true);
    m_drives_table_widget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    // m_drives_table_widget->setRowCount(mountedVolumes.size());

    auto header = m_drives_table_widget->horizontalHeader();
    m_drives_table_widget->setColumnCount(4);
    m_drives_table_widget->setHorizontalHeaderLabels(
        {"Drive", "FS Type", "Size", "Mount Point"});

    header->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);

    for (int i=1; i < 4;i++)
        header->setSectionResizeMode(i, QHeaderView::ResizeMode::ResizeToContents);
    // header->setStretchLastSection(true);
    m_drives_table_widget->setEditTriggers(QTableWidget::EditTrigger::NoEditTriggers);
    // header->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    this->setLayout(m_layout);

    connect(m_drives_watcher, &QFileSystemWatcher::directoryChanged, this,
            [&](const QString &path) { loadDrives(); });

    loadDrives();
    m_drives_watcher->addPath("/dev/");
    connect(m_drives_table_widget, &QTableWidget::cellDoubleClicked, this,
            &DriveWidget::cellDoubleClicked);

    QHBoxLayout *btn_layout = new QHBoxLayout();

    btn_layout->addWidget(m_mount_btn);
    btn_layout->addWidget(m_unmount_btn);
    btn_layout->addStretch(1);
    m_layout->addWidget(new QLabel("Double click to mount/load a drive"));
    m_layout->addLayout(btn_layout);

    // m_unmount_btn->setEnabled(false);
    // m_mount_btn->setEnabled(false);

    m_drives_table_widget->setSelectionMode(
        QAbstractItemView::SelectionMode::SingleSelection);

    connect(m_drives_table_widget, &QTableWidget::currentCellChanged, this,
            [&](const int &crow, const int &ccol, const int &prow, const int &pcol) {
            if (crow == -1)
            return;
            auto item = m_drives_table_widget->item(crow, 3);
            auto isMounted = !item->text().isEmpty();
            if (isMounted) {
            m_unmount_btn->setEnabled(true);
            m_mount_btn->setEnabled(false);
            } else {
            m_mount_btn->setEnabled(true);
            m_unmount_btn->setEnabled(false);
            }
            });

    connect(m_mount_btn, &QPushButton::clicked, this, [&]() {
        int row = m_drives_table_widget->currentRow();
        auto mountPoint = m_drives_table_widget->item(row, 3)->text();
        auto driveName = m_drives_table_widget->item(row, 0)->text();
        driveName = QString("/dev") + QDir::separator() + driveName;
        if (mountPoint.isEmpty() || mountPoint.isNull()) {
            emit driveMountRequested(driveName);
        } else
        emit driveLoadRequested(mountPoint);
    });

    connect(m_unmount_btn, &QPushButton::clicked, this, [&]() {
        int row = m_drives_table_widget->currentRow();
        auto mountPoint = m_drives_table_widget->item(row, 3)->text();
        auto driveName = m_drives_table_widget->item(row, 0)->text();
        driveName = QString("/dev") + QDir::separator() + driveName;
        if (mountPoint.isEmpty() || mountPoint.isNull()) {
            return;
        } else
        emit driveUnmountRequested(driveName);
    });
}

void DriveWidget::loadDrives() noexcept {

    uint i = 0;
    m_drives_table_widget->setRowCount(0);
    auto driveList = utils::getDrives();

    for (const auto &drive : driveList) {

        auto name = new QTableWidgetItem(QIcon::fromTheme("drive"), drive.name);
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
