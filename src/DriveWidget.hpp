#pragma once

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QStorageInfo>
#include <QTableWidget>
#include <QHeaderView>
#include <QLocale>
#include <QList>
#include <QFileSystemWatcher>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

#include "utils.hpp"

class DriveWidget : public QDialog {
Q_OBJECT
public:
  DriveWidget(QWidget *parent = nullptr);

    void show() noexcept {
        emit visibilityChanged(true);
        QDialog::show();
    }

    void hide() noexcept {
        emit visibilityChanged(true);
        QDialog::hide();
    }

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
    QPushButton *m_close_btn = new QPushButton("Close");
    QPushButton *m_mount_btn = new QPushButton("Mount");
    QPushButton *m_unmount_btn = new QPushButton("Unmount");
};