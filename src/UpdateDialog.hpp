#pragma once

#include "pch/pch_updatedialog.hpp"

class UpdateDialog : public QDialog {

public:
    explicit UpdateDialog(QString version, QWidget *parent = nullptr);

private:
    void fetch_version_from_repo() noexcept;
    QVBoxLayout *m_layout = new QVBoxLayout(this);
    WaitingSpinnerWidget *m_spinner = new WaitingSpinnerWidget(this);
    QLabel *m_label = new QLabel("Checking for update...");
    QLabel *m_version_label = new QLabel();
    QString m_version;
    QPushButton *m_close_btn = new QPushButton("Close");
};
