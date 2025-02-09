#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

class CompletionDelegate : public QStyledItemDelegate {
public:
    explicit CompletionDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    inline void setLineNumbers(const bool &state) noexcept {
        m_showLineNumbers = state;
    }

    inline bool lineNumberShown() noexcept { return m_showLineNumbers; }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        if (!index.isValid()) return;

        painter->save();

        // Check if the row is selected
        bool isSelected = option.state & QStyle::State_Selected;

        QRect rect = option.rect;
        // Draw background for selection
        if (isSelected) {
            painter->fillRect(rect, option.palette.highlight());
        }

        QRect textRect;
        if (m_showLineNumbers) {
            int lineNumber = index.row() + 1;

            // Draw row number in a separate left margin
            QRect numberRect(rect.x(), rect.y(), 40, rect.height()); // Width 40px for numbers
            painter->setPen(Qt::gray);
            painter->drawText(numberRect, Qt::AlignLeft | Qt::AlignVCenter, QString::number(lineNumber));

            // Draw separator line
            painter->setPen(Qt::lightGray);
            painter->drawLine(numberRect.topRight(), numberRect.bottomRight());
            textRect = rect.adjusted(50, 0, 0, 0);  // Shift text to the right
        } else {
            textRect = rect.adjusted(10, 0, 0, 0);
        }

        // Draw the main item text
        painter->setPen(option.palette.color(QPalette::Text));
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        return QSize(size.width() + 40, size.height());  // Increase width for row number
    }

private:
    bool m_showLineNumbers = false;
};
