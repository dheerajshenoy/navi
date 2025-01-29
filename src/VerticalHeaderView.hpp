#pragma once

#include <QHeaderView>
#include <QPainter>
#include "FileSystemModel.hpp"

class VerticalHeaderView : public QHeaderView {
    Q_OBJECT

    public:
    explicit VerticalHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr)
    : QHeaderView(orientation, parent),
    m_cursor_foreground_color(palette().color(foregroundRole())),
    m_cursor_background_color(palette().color(backgroundRole())) {}

    inline void setBorder(const bool &state) noexcept {
        m_border = state;
    }

    inline bool hasBorder() noexcept { return m_border; }

    void set_cursor_foreground(const QString &color) noexcept {
        m_cursor_foreground_color = color;
    }

    void set_cursor_background(const QString &color) noexcept {
        m_cursor_background_color = color;
    }

    void set_border(const bool &state) noexcept {
        m_border = state;
    }

    std::string get_cursor_foreground() noexcept {
        return m_cursor_foreground_color.name().toStdString();
    }

    std::string get_cursor_background() noexcept {
        return m_cursor_background_color.name().toStdString();
    }

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override {
        QStyleOptionHeader option;
        initStyleOption(&option);
        option.rect = rect;
        option.section = logicalIndex;

        auto _model = dynamic_cast<FileSystemModel*>(model());

        // Draw text and other default elements
        if (_model) {
            // Get background color from model
            if (logicalIndex == _model->get_cursor_row()) {
                painter->fillRect(rect, m_cursor_background_color);
                painter->setPen(m_cursor_foreground_color);
            } else {
                QVariant backgroundData = _model->headerData(logicalIndex, orientation(), Qt::BackgroundRole);

                if (backgroundData.canConvert<QBrush>()) {
                    QBrush backgroundBrush = backgroundData.value<QBrush>();
                    painter->fillRect(rect, backgroundBrush);
                } else {
                    // Fallback to default background if no custom color is provided
                    painter->fillRect(rect, palette().brush(backgroundRole()));
                }

                // Get foreground color from model
                QVariant foregroundData = _model->headerData(logicalIndex, orientation(), Qt::ForegroundRole);
                if (foregroundData.canConvert<QBrush>()) {
                    QBrush foregroundBrush = foregroundData.value<QBrush>();
                    painter->setPen(foregroundBrush.color());
                } else {
                    // Fallback to default foreground if no custom color is provided
                    painter->setPen(palette().color(foregroundRole()));
                }
            }

            painter->drawText(rect, Qt::AlignCenter, QString::number(logicalIndex));
            // painter->drawText(rect, Qt::AlignCenter, QString::number(relativeRow));

            // Draw border
            if (m_border) {
                painter->setPen(Qt::gray);
                painter->drawRect(rect.adjusted(0, 0, -1, -1));
            }

        }
    }

private:
    QColor m_cursor_foreground_color;
    QColor m_cursor_background_color;
    bool m_border;
};
