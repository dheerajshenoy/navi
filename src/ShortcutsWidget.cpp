#include "ShortcutsWidget.hpp"

ShortcutsWidget::ShortcutsWidget(const QList<Keybind> &keybinds, QWidget *parent) : QWidget(parent) {
    m_model->setKeyBinds(keybinds);
    m_layout->addWidget(m_line_edit);
    m_layout->addWidget(m_table_view);

    m_table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    m_line_edit->setPlaceholderText("Search");

    // Create the proxy model for filtering
    proxyModel->setSourceModel(m_model);
    m_table_view->setModel(proxyModel);

    // When the search button is clicked, update the filter on the proxy model
    connect(m_line_edit, &QLineEdit::textChanged, [this](const QString &text) {
        proxyModel->setFilterRegularExpression(
                                               QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
    });
    this->setLayout(m_layout);
}