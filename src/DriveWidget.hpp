#pragma once

#include "pch/pch_drivewidget.hpp"
#include "HookManager.hpp"
#include "../external/KDDockWidgets/src/qtwidgets/views/DockWidget.h"
#include "utils.hpp"

class DriveWidget : public KDDockWidgets::QtWidgets::DockWidget {
Q_OBJECT
public:
  DriveWidget();

signals:
    void driveLoadRequested(const QString &driveName);
    void driveMountRequested(const QString &driveName);
    void driveUnmountRequested(const QString &driveName);
    void visibilityChanged(const bool &state);

private:
    void loadDrives() noexcept;
    void cellDoubleClicked(int row, int col) noexcept;
    QVBoxLayout *m_layout = new QVBoxLayout();
    QTableWidget *m_drives_table_widget = new QTableWidget();
    QFileSystemWatcher *m_drives_watcher = new QFileSystemWatcher();
    QPushButton *m_mount_btn = new QPushButton("Mount");
    QPushButton *m_unmount_btn = new QPushButton("Unmount");
};
