#include "BookmarkWidget.hpp"

BookmarkWidget::BookmarkWidget(BookmarkManager *manager, QWidget *parent)
: m_manager(manager), QWidget(parent) {

    m_layout = new QVBoxLayout();
    m_table_widget = new QTableWidget();
    m_layout->addWidget(m_table_widget);
    m_table_widget->setEditTriggers(QTableWidget::EditTrigger::NoEditTriggers);
    this->setLayout(m_layout);

    if (!m_manager)
        return;

    auto bookmarks = m_manager->getBookmarks();
    m_table_widget->setColumnCount(2);
    m_table_widget->setRowCount(bookmarks.size());

    if (bookmarks.size() == 0)
        return;

    uint row = 0;
    for (auto i = bookmarks.cbegin(), end = bookmarks.cend(); i != end; i++) {
        auto key = new QTableWidgetItem(i.key());
        auto value = new QTableWidgetItem(i.value().file_path);
        m_table_widget->setItem(row, 0, key);
        m_table_widget->setItem(row, 1, value);
        row++;
    }

    QPushButton *m_close_btn = new QPushButton("Close");
    connect(m_close_btn, &QPushButton::clicked, this, [&]() { this->close(); });
    m_layout->addWidget(m_close_btn);

}