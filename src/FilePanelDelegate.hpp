#pragma once

#include "pch/pch_filepaneldelegate.hpp"
#include "FileSystemModel.hpp"
#include "TableView.hpp"

class FilePanelDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
    FilePanelDelegate(QObject *parent = nullptr) :
    QStyledItemDelegate(parent),
    m_symlink_font(QApplication::font()),
    m_cursor_font(QApplication::font()),
    m_symlink_foreground("red"){}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override {
        if (!index.isValid())
            return;

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Get the text and icon
        QString fileName = index.data(static_cast<int>(FileSystemModel::Role::FileName)).toString();
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

        // Draw the background
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

        if (index.row() == index.data(static_cast<int>(FileSystemModel::Role::Cursor))) {
            painter->fillRect(opt.rect, m_cursor_background);
        } else if (option.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, opt.palette.color(QPalette::Highlight));
        }

        if (index.column() == 1) {

            QRect textRect = opt.rect;

            painter->setPen(opt.palette.color(QPalette::Text));
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, opt.text);
        }

        if (index.column() == 0) {
            // Calculate rectangles
            QRect iconRect = opt.rect;
            iconRect.setWidth(opt.decorationSize.width());

            QRect textRect = opt.rect;

            // Draw the icon if enabled in model
            if (!icon.isNull()) {
                icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
                textRect.setLeft(iconRect.right() + 4); // Add some padding after icon
            } else {
                textRect.setLeft(4);
            }


            QString symlinkTargetName = index.data(static_cast<int>(FileSystemModel::Role::Symlink)).toString();

            if (!symlinkTargetName.isEmpty()) {

                // Draw filename
                painter->setPen(opt.palette.color(QPalette::Text));
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, fileName);

                // Draw separator
                int fileNameWidth = painter->fontMetrics().horizontalAdvance(fileName);
                painter->drawText(textRect.adjusted(fileNameWidth + 5, 0, 0, 0),
                                  Qt::AlignLeft | Qt::AlignVCenter, m_symlink_separator);

                painter->save();

                // Draw target path in the model's symlink color
                painter->setFont(m_symlink_font);

                int sepWidth = painter->fontMetrics().horizontalAdvance(m_symlink_separator);
                int targetTextWidth = painter->fontMetrics().horizontalAdvance(symlinkTargetName);
                int targetStartX = textRect.left() + fileNameWidth + sepWidth + 4;
                QRect targetRect(targetStartX, textRect.top(),
                                 targetTextWidth,
                                 textRect.height());

                if (!m_symlink_background.isEmpty()) {
                    painter->fillRect(targetRect, QColor(m_symlink_background));
                }

                painter->setPen(QPen(QColor(m_symlink_foreground)));
                painter->drawText(textRect.adjusted(fileNameWidth + sepWidth + 10, 0, 0, 0),
                                  Qt::AlignLeft | Qt::AlignVCenter, symlinkTargetName);

                painter->restore();
            } else {
                // For non-symlink items, draw the full text
                painter->setPen(opt.palette.color(QPalette::Text));
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, fileName);
            }
        }
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

    inline void set_cursor_font(const QString &family) noexcept {
        m_cursor_font.setFamily(family);
    }

    inline QString get_cursor_font() noexcept {
        return m_cursor_font.family();
    }

    inline void set_cursor_foreground(const QString &fg) noexcept {
        m_cursor_foreground = fg;
    }

    inline QString get_cursor_foreground() noexcept {
        return m_cursor_foreground;
    }

    inline void set_cursor_background(const QString &bg) noexcept {
        m_cursor_background = bg;
    }

    inline QString get_cursor_background() noexcept {
        return m_cursor_background;
    }

    inline void set_cursor_italic(const bool &state) noexcept {
        m_cursor_font.setItalic(state);
    }

    inline bool get_cursor_italic() noexcept {
        return m_cursor_font.italic();
    }

    inline void set_cursor_bold(const bool &state) noexcept {
        m_cursor_font.setBold(state);
    }

    inline bool get_cursor_bold() noexcept {
        return m_cursor_font.bold();
    }

    inline void set_cursor_underline(const bool &state) noexcept {
        m_cursor_font.setUnderline(state);
    }

    inline bool get_cursor_underline() noexcept {
        return m_cursor_font.underline();
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        return QSize(size.width(), qMax(size.height(), option.decorationSize.height() + 4));
    }

private:
    QFont m_symlink_font, m_cursor_font;
    QString m_symlink_foreground, m_symlink_background, m_symlink_separator = " ⟶ ",
    m_cursor_foreground, m_cursor_background = "#444444";
};
