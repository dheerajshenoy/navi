#pragma once

#include "pch/pch_folderpropertywidget.hpp"
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
