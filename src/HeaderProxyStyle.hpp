#pragma once

#include <QProxyStyle>
#include <QPainter>
#include <QStyleOptionHeader>

class HeaderProxyStyle : public QProxyStyle {
public:
    explicit HeaderProxyStyle(QStyle* style = nullptr) : QProxyStyle(style) {}

    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const override {
        if (element == CE_HeaderSection) {
            const QStyleOptionHeader* headerOption = qstyleoption_cast<const QStyleOptionHeader*>(option);
            if (headerOption) {
                // Customize background color for the header section
                // QColor headerColor = QColor("#FFCC99");
                // painter->fillRect(headerOption->rect, headerColor);

                // Section Border
                painter->setPen(Qt::gray);
                painter->drawRect(headerOption->rect.adjusted(0, 0, -1, -1));

                return; // Skip default drawing since we handled it
            }
        }

        // Default drawing for other elements
        QProxyStyle::drawControl(element, option, painter, widget);
    }

private:
    // Draws the header text within the customized background
    void drawHeaderLabel(const QStyleOptionHeader* option, QPainter* painter, const QWidget* widget) const {
        painter->save();
        painter->setPen(option->palette.color(QPalette::ButtonText));
        QRect textRect = option->rect;
        QString text = option->text;

        // Align the text in the center of the header section
        painter->drawText(textRect, Qt::AlignCenter, text);
        painter->restore();
    }
};