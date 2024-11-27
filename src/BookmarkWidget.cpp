#include "BookmarkWidget.hpp"

BookmarkWidget::BookmarkWidget(BookmarkManager *manager, QWidget *parent)
: QDialog(parent), m_manager(manager) {

    m_layout = new QVBoxLayout();
    m_table_widget = new QTableWidget();

    m_table_widget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    m_table_widget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    m_layout->addWidget(m_table_widget);
    m_table_widget->setEditTriggers(QTableWidget::EditTrigger::NoEditTriggers);
    auto header = m_table_widget->horizontalHeader();

    this->setLayout(m_layout);

    connect(m_table_widget, &QTableWidget::itemDoubleClicked, this,
            [&](const QTableWidgetItem *item) {
              if (item)
                emit bookmarkGoRequested(
                    m_table_widget->item(item->row(), 0)->text());
            });

    QPushButton *m_close_btn = new QPushButton("Close");
    connect(m_close_btn, &QPushButton::clicked, this, [&]() { this->close(); });
    m_layout->addWidget(m_close_btn);

    if (!m_manager)
        return;

    m_table_widget->setColumnCount(3);
    m_table_widget->setHorizontalHeaderLabels({
        "Bookmark Name", "Bookmark File Path",
        "Highlight Only?"});

    for (int i=0; i < m_table_widget->columnCount(); i++)
      header->setSectionResizeMode(i, QHeaderView::ResizeMode::ResizeToContents);

    header->setStretchLastSection(true);

    loadBookmarks();
}

void BookmarkWidget::loadBookmarks() noexcept {
    auto bookmarks = m_manager->getBookmarks();
    m_table_widget->setRowCount(bookmarks.size());

    if (bookmarks.size() == 0)
        return;

    uint row = 0;
    QString highlight;
    for (auto i = bookmarks.cbegin(), end = bookmarks.cend(); i != end; i++) {
        auto key = new QTableWidgetItem(i.key());
        auto value = new QTableWidgetItem(i.value().file_path);
        if (i.value().highlight_only)
            highlight = "true";
        else
            highlight = "false";

        auto highlight_item = new QTableWidgetItem(highlight);
        m_table_widget->setItem(row, 0, key);
        m_table_widget->setItem(row, 1, value);
        m_table_widget->setItem(row, 2, highlight_item);
        row++;
    }

}