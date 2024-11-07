#include "NaviWindow.hpp"

NaviWindow::NaviWindow(QWidget *parent) : QWidget(parent) {
    m_splitter->addWidget(m_file_panel);
    m_splitter->addWidget(m_preview_panel);
    m_layout->addWidget(m_splitter);
}