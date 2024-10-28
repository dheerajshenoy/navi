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

#include "utils.hpp"

class FilePropertyWidget : public QDialog {

public:
    explicit FilePropertyWidget(const QString& itemPath, QWidget *parent = nullptr);

private:
    QFormLayout *m_layout = nullptr;
    QLocale m_locale;
};