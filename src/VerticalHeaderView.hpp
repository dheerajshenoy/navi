#pragma once

#include <QHeaderView>
#include <QPainter>

class VerticalHeaderView : public QHeaderView {
    Q_OBJECT

public:
    explicit VerticalHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr)
        : QHeaderView(orientation, parent), m_backgroundColor(Qt::lightGray) {}

    void setBackgroundColor(const QColor &color) {
        m_backgroundColor = color;
        update(); // Trigger a repaint
    }

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override {
        QStyleOptionHeader option;
        initStyleOption(&option);
        option.rect = rect;
        option.section = logicalIndex;

        // Draw text and other default elements
        if (model()) {
            // Get background color from model
            QVariant backgroundData = model()->headerData(logicalIndex, orientation(), Qt::BackgroundRole);
            if (backgroundData.canConvert<QBrush>()) {
                QBrush backgroundBrush = backgroundData.value<QBrush>();
                painter->fillRect(rect, backgroundBrush);
            } else {
                // Fallback to default background if no custom color is provided
                painter->fillRect(rect, palette().brush(backgroundRole()));
            }


            // Optional: Draw border
            painter->setPen(Qt::gray);
            painter->drawRect(rect.adjusted(0, 0, -1, -1));

            QVariant text = model()->headerData(logicalIndex, orientation(),
                                                Qt::DisplayRole);

            // Get foreground color from model
            QVariant foregroundData = model()->headerData(logicalIndex, orientation(), Qt::ForegroundRole);
            if (foregroundData.canConvert<QBrush>()) {
                QBrush foregroundBrush = foregroundData.value<QBrush>();
                painter->setPen(foregroundBrush.color());
            } else {
                // Fallback to default foreground if no custom color is provided
                painter->setPen(palette().color(foregroundRole()));
            }

            painter->drawText(rect, Qt::AlignCenter, text.toString());

        }
    }

private:
    QColor m_backgroundColor;
};