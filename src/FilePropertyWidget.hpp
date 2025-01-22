#pragma once

#include "pch/pch_filepropertywidget.hpp"
#include "utils.hpp"

class FilePropertyWidget : public QDialog {

public:
    explicit FilePropertyWidget(const QString& itemPath, QWidget *parent = nullptr);
    ~FilePropertyWidget();

private:

    QFormLayout *m_layout = nullptr;
    QLocale m_locale;

    bool m_symbolic = false;
    QString m_symbolic_orig_filename,
    m_symbolic_orig_filepath;

    QFutureWatcher<utils::FolderInfo> *future_watcher = new QFutureWatcher<utils::FolderInfo>(this);
    QFuture<utils::FolderInfo> future;
};
