#include "CompletionPopup.hpp"
#include <qobjectdefs.h>

CompletionPopup::CompletionPopup(LineEdit* parent)
    : QFrame(parent), m_lineEdit(parent),
    m_model(new QStringListModel(this)),
    m_filter_model(new CompletionFilterModel(this)),
    m_completion_delegate(new CompletionDelegate(m_lineEdit))
{
    setWindowFlags(Qt::ToolTip);
    setFocusPolicy(Qt::NoFocus);

    m_listView = new QListView();
    m_listView->setEditTriggers(QListView::EditTrigger::NoEditTriggers);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_listView);
    setLayout(layout);

    connect(m_listView, &QListView::clicked, this,
            &CompletionPopup::selectItem);

    m_filter_model->setSourceModel(m_model);
    m_listView->setModel(m_filter_model);
    m_lineEdit->installEventFilter(this);
    m_listView->setItemDelegate(m_completion_delegate);

}

void CompletionPopup::updateCompletions(const QString &text) noexcept {
    if (text.isEmpty()) {
        m_filter_model->setFilterRegularExpression("");
    } else {
        m_filter_model->setFilterFixedString(text);
    }

    auto count = m_filter_model->rowCount();
    if (count == 0) {
        hide();
    } else {
        emit matchCountSignal(count, m_total_completions_count);
        showPopup();
    }
}

void CompletionPopup::showPopup() noexcept {

    auto g = m_lineEdit->mapToGlobal(QPoint(0, m_lineEdit->height()));
    move(g.x(), g.y() - this->height() - m_lineEdit->height());
    show();
    raise();
}

void CompletionPopup::updatePopupGeometry() noexcept {
    auto g = m_lineEdit->mapToGlobal(QPoint(0, m_lineEdit->height()));
    this->setGeometry(g.x(), g.y() - this->height() - m_lineEdit->height(), m_lineEdit->width(),
                      this->height());
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
        }

        else if (event->type() == QEvent::Resize) {
            updatePopupGeometry();
        }

        else if (event->type() == QEvent::FocusOut) {
            hide();  // Hide when QLineEdit loses focus
        }
    }
    return QFrame::eventFilter(obj, event);
}

void CompletionPopup::selectItem(const QModelIndex &index) noexcept {

    if (!index.isValid())
        return;

    QModelIndex sourceIndex = m_filter_model->mapFromSource(index);

    if (!sourceIndex.isValid())
        return;

    QString itemText = m_filter_model->data(sourceIndex).toString();
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
    m_total_completions_count = completions.size();

}
