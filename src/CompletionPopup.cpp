#include "CompletionPopup.hpp"

CompletionPopup::CompletionPopup(LineEdit* parent)
    : QFrame(parent), m_lineEdit(parent) {
    setWindowFlags(Qt::ToolTip);
    setFocusPolicy(Qt::NoFocus);

    m_listWidget = new QListWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_listWidget);
    setLayout(layout);

    connect(m_listWidget, &QListWidget::itemClicked, this,
            &CompletionPopup::selectItem);
    m_lineEdit->installEventFilter(this);

    connect(m_lineEdit, &QLineEdit::textChanged, this,
            [&](const QString &text) {
                if (text.isEmpty())
                    setCompletions(m_initial_completions);
            });
}

void CompletionPopup::setCompletions(const QStringList &completions) noexcept {
    m_current_completions = completions;
    m_listWidget->clear();
    // m_listWidget->setUpdatesEnabled(false);
    for (const auto& item : completions)
        m_listWidget->addItem(item);
    // m_listWidget->setUpdatesEnabled(true);
}

void CompletionPopup::updateCompletions(const QString &text) noexcept {
    m_listWidget->clear();

    if (text.isEmpty()) {
        hide();
        return;
    }

    QStringList filteredCompletions;
    for (const auto& item : m_current_completions) {
        if (item.startsWith(text, m_case_sensitivity)) {
            filteredCompletions.append(item);
        }
    }

    if (filteredCompletions.isEmpty()) {
        hide();
        return;
    }

    for (const auto& match : filteredCompletions) {
        m_listWidget->addItem(match);
    }

    showPopup();
}

void CompletionPopup::showPopup() noexcept {

    if (!m_listWidget->count())
        return;

    auto g = m_lineEdit->mapToGlobal(QPoint(0, m_lineEdit->height()));
    setGeometry(g.x(), g.y() - m_popupHeight - m_lineEdit->height(), m_lineEdit->width(), m_popupHeight);
    show();
    raise();
}

bool CompletionPopup::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_lineEdit) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (isVisible()) {
                if (keyEvent->key() == Qt::Key_Up) {
                    int row = m_listWidget->currentRow();
                    if (row > 0) {
                        m_listWidget->setCurrentRow(row - 1);
                    }
                    return true;
                } else if (keyEvent->key() == Qt::Key_Down) {
                    int row = m_listWidget->currentRow();
                    if (row < m_listWidget->count() - 1) {
                        m_listWidget->setCurrentRow(row + 1);
                    }
                    return true;
                } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                    selectItem(m_listWidget->currentItem());
                    return true;
                } else if (keyEvent->key() == Qt::Key_Escape) {
                    hide();
                    return true;
                }
            }
        } else if (event->type() == QEvent::FocusOut) {
            hide();  // Hide when QLineEdit loses focus
        }
    }
    return QFrame::eventFilter(obj, event);
}

void CompletionPopup::selectItem(QListWidgetItem* item) noexcept {
    if (!item) return;
    QString currentText = m_lineEdit->text();
    int lastSpace = currentText.lastIndexOf(' ');
    if (lastSpace == -1) {
        m_lineEdit->setText(item->text() + " ");
    } else {
        m_lineEdit->setText(currentText.left(lastSpace + 1) + item->text() + " ");
    }
    emit m_lineEdit->spacePressed();
    hide();
}
