#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include "FileSystemModel.hpp"
#include <QApplication>
#include <QFont>
#include <QTableView>

class FilePanelDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
    FilePanelDelegate(QObject *parent = nullptr)
    :   QStyledItemDelegate(parent),
    m_symlink_foreground("red")
    {
        m_symlink_font = QApplication::font();
}

inline void set_symlink_separator(const QString &sep) noexcept {
    m_symlink_separator = sep;
}

inline std::string get_symlink_separator() noexcept {
    return m_symlink_separator.toStdString();
}

inline void set_symlink_foreground(const QString &foreground) noexcept {
    m_symlink_foreground = foreground;
}

inline std::string get_symlink_foreground() const noexcept {
    return m_symlink_foreground.toStdString();
}

inline void set_symlink_background(const QString &background) noexcept {
    m_symlink_background = background;
}

inline std::string get_symlink_background() const noexcept {
    return m_symlink_background.toStdString();
}

inline void set_symlink_italic(const bool &state) noexcept {
    m_symlink_font.setItalic(state);
}

inline bool get_symlink_italic() const noexcept {
    return m_symlink_font.italic();
}

inline void set_symlink_bold(const bool &state) noexcept {
    m_symlink_font.setBold(state);
}

inline bool get_symlink_bold() noexcept {
    return m_symlink_font.bold();
}

inline void set_symlink_underline(const bool &state) noexcept {
    m_symlink_font.setUnderline(state);
}

inline bool get_symlink_underline() noexcept {
    return m_symlink_font.underline();
}

inline std::string get_symlink_font() noexcept {
    return m_symlink_font.family().toStdString();
}

inline void set_symlink_font(const QString &name) noexcept {
    m_symlink_font.setFamily(name);
}

inline void set_symlink_font_size(const int &size) noexcept {
    m_symlink_font.setPixelSize(size);
}

inline int get_symlink_font_size() noexcept {
    return m_symlink_font.pixelSize();
}

void paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const override {
    if (!index.isValid())
        return;

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Get the text and icon
    QString displayText = index.data(Qt::DisplayRole).toString();
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

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
    int sepIndex = displayText.indexOf(m_symlink_separator);
    if (sepIndex != -1) {
        // Split the text into filename and target
        QString fileName = displayText.left(sepIndex);
        QString targetPath = displayText.mid(sepIndex + m_symlink_separator.length());

        // Draw filename
        painter->setPen(opt.palette.color(QPalette::Text));
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, fileName);

        // Draw separator
        int fileNameWidth = painter->fontMetrics().horizontalAdvance(fileName);
        painter->drawText(textRect.adjusted(fileNameWidth, 0, 0, 0),
                          Qt::AlignLeft | Qt::AlignVCenter, m_symlink_separator);

        painter->save();

        // Draw target path in the model's symlink color
        painter->setFont(m_symlink_font);

        int sepWidth = painter->fontMetrics().horizontalAdvance(m_symlink_separator);
        int targetTextWidth = painter->fontMetrics().horizontalAdvance(targetPath);
        int targetStartX = textRect.left() + fileNameWidth + sepWidth + 4;
        QRect targetRect(targetStartX, textRect.top(),
                         targetTextWidth,
                         textRect.height());

        if (!m_symlink_background.isEmpty()) {
            painter->fillRect(targetRect, QColor(m_symlink_background));
        }

        painter->setPen(QPen(QColor(m_symlink_foreground)));
        painter->drawText(textRect.adjusted(fileNameWidth + sepWidth + 4, 0, 0, 0),
                          Qt::AlignLeft | Qt::AlignVCenter, targetPath);

        painter->restore();
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

private:
QFont m_symlink_font;
QString m_symlink_foreground, m_symlink_background, m_symlink_separator = "⟶";
};
