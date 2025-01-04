#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include "FileSystemModel.hpp"
#include <QApplication>

class FilePanelDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
    FilePanelDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        if (!index.isValid())
            return;

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Get the text and icon
        QString displayText = index.data(Qt::DisplayRole).toString();
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

        QString separator = "⟶";

        // Draw the background
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

        // Calculate rectangles
        QRect iconRect = opt.rect;
        iconRect.setWidth(opt.decorationSize.width());

        QRect textRect = opt.rect;
        textRect.setLeft(iconRect.right() + 4); // Add some padding after icon

        // Draw the icon if enabled in model
        if (!icon.isNull()) {
            icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }

        // For symlinks, split and color the text
        int sepIndex = displayText.indexOf(separator);
        if (sepIndex != -1) {
            // Split the text into filename and target
            QString fileName = displayText.left(sepIndex);
            QString targetPath = displayText.mid(sepIndex + separator.length());

            // Draw filename
            painter->setPen(opt.palette.color(QPalette::Text));
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, fileName);

            // Draw separator
            int fileNameWidth = painter->fontMetrics().horizontalAdvance(fileName);
            painter->drawText(textRect.adjusted(fileNameWidth, 0, 0, 0),
                              Qt::AlignLeft | Qt::AlignVCenter, separator);

            // Draw target path in the model's symlink color
            painter->setPen(QPen(QColor("red")));
            int sepWidth = painter->fontMetrics().horizontalAdvance(separator);
            painter->drawText(textRect.adjusted(fileNameWidth + sepWidth, 0, 0, 0),
                              Qt::AlignLeft | Qt::AlignVCenter, targetPath);
        } else {
            // For non-symlink items, draw the full text
            painter->setPen(opt.palette.color(QPalette::Text));
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, displayText);
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        return QSize(size.width(), qMax(size.height(), option.decorationSize.height() + 4));
    }
};
