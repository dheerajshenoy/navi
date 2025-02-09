#include "CompletionPopup.hpp"

CompletionPopup::CompletionPopup(LineEdit* parent)
    : QFrame(parent), m_lineEdit(parent),
    m_model(new QStringListModel(this)),
    m_filter_model(new CompletionFilterModel(this))
{
    setWindowFlags(Qt::ToolTip);
    setFocusPolicy(Qt::NoFocus);

    m_listView = new QListView();
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_listView);
    setLayout(layout);

    connect(m_listView, &QListView::clicked, this,
            &CompletionPopup::selectItem);

    m_listView->setModel(m_filter_model);
    m_filter_model->setSourceModel(m_model);

    m_lineEdit->installEventFilter(this);

}

void CompletionPopup::updateCompletions(const QString &text) noexcept {
    if (text.isEmpty()) {
        m_filter_model->setFilterRegularExpression("");
    } else {
        m_filter_model->setFilterFixedString(text);
    }

    if (m_filter_model->rowCount() == 0) {
        hide();
        return;
    }

    showPopup();
}

void CompletionPopup::showPopup() noexcept {

    if (m_model->rowCount() == 0)
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
                QModelIndex currentIndex = m_listView->currentIndex();
                int row = currentIndex.isValid() ? currentIndex.row() : -1;
                int maxRow = m_filter_model->rowCount();  // Get filtered row count

                if (keyEvent->key() == Qt::Key_Up) {
                    if (row > 0) {
                        QModelIndex newIndex = m_filter_model->index(row - 1, 0);
                        m_listView->setCurrentIndex(newIndex);
                    }
                    return true;
                } else if (keyEvent->key() == Qt::Key_Down) {
                    if (row < maxRow - 1) {  // Ensure not exceeding the filtered items
                        QModelIndex newIndex = m_filter_model->index(row + 1, 0);
                        m_listView->setCurrentIndex(newIndex);
                    }
                    return true;
                } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                    if (currentIndex.isValid()) {
                        QModelIndex sourceIndex = m_filter_model->mapToSource(currentIndex);
                        selectItem(sourceIndex);
                    }
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

void CompletionPopup::selectItem(const QModelIndex &index) noexcept {
    if (!index.isValid())
        return;

    QString itemText = m_model->data(index).toString();
    QString currentText = m_lineEdit->text();
    int lastSpace = currentText.lastIndexOf(' ');
    if (lastSpace == -1) {
        m_lineEdit->setText(itemText + " ");
    } else {
        m_lineEdit->setText(currentText.left(lastSpace + 1) + itemText + " ");
    }
    emit m_lineEdit->spacePressed();
    hide();
}

void CompletionPopup::setCompletions(const QStringList &completions) noexcept {
    m_model->setStringList(completions);
    m_filter_model->invalidate();
}

void CompletionPopup::updateAndShowPopup() noexcept {
    auto text = m_lineEdit->text();
    if (text.isEmpty()) {
        updateCompletions("");
        return;
    }

    auto split = text.split(" ", Qt::SkipEmptyParts);
    if (split.length() == 1) {
        updateCompletions("");
    } else {
        auto last = split.last();
        updateCompletions(last);
    }
}
