#pragma once

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileInfo>
#include <QLocale>
#include <QMimeDatabase>
#include <QIcon>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QPromise>
#include <QFutureWatcher>
#include <QDirIterator>
#include "utils.hpp"

class FolderPropertyWidget : public QDialog {

public:

    explicit FolderPropertyWidget(const QString& folderPath, QWidget *parent = nullptr);
    ~FolderPropertyWidget();

private:

    utils::FolderInfo getFolderInfo(const QString &folderPath) noexcept;
    QFormLayout *m_layout = nullptr;

    QLocale locale;
    QFutureWatcher<utils::FolderInfo> *future_watcher = new QFutureWatcher<utils::FolderInfo>(this);
    QFuture<utils::FolderInfo> future;
};