#pragma once

#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include "FilePanel.hpp"
#include "PreviewPanel.hpp"

class NaviWindow : public QWidget {

public:
    NaviWindow(QWidget *parent = nullptr);

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    FilePanel *m_file_panel = nullptr;
    PreviewPanel *m_preview_panel = nullptr;
    QSplitter *m_splitter = nullptr;
};