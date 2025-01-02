#include "CustomCompleter.hpp"

CustomCompleter::CustomCompleter(QWidget *parent)
    : QWidget(parent), listWidget(new QListWidget(this)), attachedWidget(nullptr) {
    listWidget->setWindowFlags(Qt::Popup);
    listWidget->setFocusPolicy(Qt::NoFocus);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->hide();
    listWidget->installEventFilter(this);   // Handle focus and event filtering
    setupConnections();
}

void CustomCompleter::setWidget(QLineEdit *widget) {
    attachedWidget = widget;
}

void CustomCompleter::setSuggestions(const QStringList &suggestions) {
    suggestionList = suggestions;
    listWidget->clear();
    listWidget->addItems(suggestionList);
}

void CustomCompleter::popup() {
    if (!attachedWidget)
        return;

    listWidget->setFocusPolicy(Qt::NoFocus);
    const QRect rect = attachedWidget->rect();
    QPoint pos = attachedWidget->mapToGlobal(QPoint(0, rect.height()));
    listWidget->setGeometry(pos.x(), pos.y(), 100, 150); // Adjust height as needed
    listWidget->show();
}

void CustomCompleter::popup_hide() noexcept {
    if (!attachedWidget || !listWidget)
        return;

    listWidget->hide();
}

void CustomCompleter::keyPressEvent(QKeyEvent *event) {
    if (listWidget->isVisible() && (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up))
    {
        listWidget->setFocus();
        QApplication::sendEvent(listWidget, event);
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

void CustomCompleter::filterSuggestions(const QString &text) {
    if (text.isEmpty())
    {
        listWidget->show();
        return;
    }

    if (listWidget)
        listWidget->clear();

    QStringList filtered;
    for (const auto &c : suggestionList) {
        if (c.startsWith(text, Qt::CaseInsensitive))
            filtered.append(c);
    }

    if (filtered.isEmpty())
    {
        listWidget->hide();
        return;
    }

    listWidget->addItems(filtered);
    popup();
}

void CustomCompleter::onItemClicked(QListWidgetItem *item) {
    if (attachedWidget) {

        int extra = item->text().length() - m_current_completion_string.length();
        attachedWidget->insert(item->text().right(extra));
        attachedWidget->setFocus();
    }

    listWidget->hide();
}

void CustomCompleter::setupConnections() {
    connect(listWidget, &QListWidget::itemClicked, this, &CustomCompleter::onItemClicked);
}

bool CustomCompleter::eventFilter(QObject *object, QEvent *event) {
    if (object == listWidget)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape)
            {
                listWidget->hide();
                return true; // Consume the event
            }
            else if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
            {
                // Handle selection
                if (listWidget->currentItem())
                {
                    onItemClicked(listWidget->currentItem());
                }
                return true; // Consume the event
            }
            else if (keyEvent->key() == Qt::Key_Down) {
                if (listWidget->currentRow() < listWidget->count() - 1)
                    listWidget->setCurrentRow(listWidget->currentRow() + 1);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Up) {
                if (listWidget->currentRow() > 0)
                    listWidget->setCurrentRow(listWidget->currentRow() - 1);
                return true;
            }
            else
            {
                QApplication::sendEvent(attachedWidget, event);
                return true;
            }
        }
    }
    return QWidget::eventFilter(object, event);
}
