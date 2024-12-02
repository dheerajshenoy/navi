#pragma once

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileInfo>
#include <QLocale>
#include <QMimeDatabase>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QPromise>
#include <QIcon>
#include "utils.hpp"

class FilePropertyWidget : public QDialog {

public:
    explicit FilePropertyWidget(const QString& itemPath, QWidget *parent = nullptr);
    ~FilePropertyWidget();

private:
    QFormLayout *m_layout = nullptr;
    QLocale m_locale;

    QFutureWatcher<utils::FolderInfo> *future_watcher = new QFutureWatcher<utils::FolderInfo>(this);
    QFuture<utils::FolderInfo> future;
};