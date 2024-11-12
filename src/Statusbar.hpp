#pragma once

#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPalette>
#include <QSizePolicy>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include "utils.hpp"

enum class MessageType { INFO = 0, WARNING, ERROR };

class Statusbar : public QWidget {
    Q_OBJECT

signals:
    void visibilityChanged(const bool &state);
    void logMessage(const QString &message, const MessageType &type);

public:
    Statusbar(QWidget *parent = nullptr);
    ~Statusbar();


    void SetVisualLineMode(const bool &state) noexcept;
    void Message(const QString &message, MessageType type = MessageType::INFO,
                 int ms = 2) noexcept;
    void SetFile(const QString &filename) noexcept;
    void UpdateFile() noexcept;
    void SetFilterMode(const bool state) noexcept;
    void SetNumItems(const int &numItems) noexcept;
    void SetSearchMatchCount(const int &totalCount) noexcept;
    void SetSearchMatchIndex(const int &matchIndex) noexcept;

    inline void SetVisualLineModeBackground(const QString &bg) {
        m_visual_line_mode_label_bg = bg;
        m_visual_line_mode_label->setStyleSheet(
            QString("background: %1; foreground: %2; padding: 2px; ")
                .arg(m_visual_line_mode_label_bg.name())
                .arg(m_visual_line_mode_label_fg.name())
                .arg(m_visual_line_mode_label_padding_string));
    }

    inline void SetVisualLineModeForeground(const QString &fg) {
        m_visual_line_mode_label_fg = fg;
        m_visual_line_mode_label->setStyleSheet(
            QString("background: %1; foreground: %2; padding: 2px; ")
                .arg(m_visual_line_mode_label_bg.name())
                .arg(m_visual_line_mode_label_fg.name())
                .arg(m_visual_line_mode_label_padding_string));
    }

    inline void SetVisualLineModeItalic(const bool &state) {
        QFont font = m_visual_line_mode_label->font();
        font.setItalic(state);
        m_visual_line_mode_label->setFont(font);
    }

    inline void SetVisualLineModeBold(const bool &state) {
        QFont font = m_visual_line_mode_label->font();
        font.setBold(state);
        m_visual_line_mode_label->setFont(font);
    }

    inline void SetVisualLineModePadding(const QString &padding) {
        m_visual_line_mode_label_padding_string = padding;
        m_visual_line_mode_label->setStyleSheet(
            QString("background: %1; foreground: %2; padding: %3; ")
                .arg(m_visual_line_mode_label_bg.name())
                .arg(m_visual_line_mode_label_fg.name())
                .arg(m_visual_line_mode_label_padding_string));
    }

    inline void SetVisualLineModeText(const QString &text) {
        m_visual_line_mode_label_text = text;
        m_visual_line_mode_label->setText(text);
    }

    void show() noexcept {
        emit visibilityChanged(true);
        QWidget::show();
    }

    void hide() noexcept {
        emit visibilityChanged(false);
        QWidget::hide();
    }


private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    QVBoxLayout *m_vert_layout = new QVBoxLayout(this);
    QLabel *m_message_label = new QLabel();
    QLabel *m_file_name_label = new QLabel();
    QLabel *m_file_size_label = new QLabel();
    QLabel *m_file_perm_label = new QLabel();
    QLabel *m_file_modified_label = new QLabel();
    QLabel *m_filter_label = new QLabel("FILTER");
    QLabel *m_num_items_label = new QLabel();
    QLabel *m_visual_line_mode_label = new QLabel("VISUAL");
    QLabel *m_search_match_label = new QLabel();

    int m_search_total_count = -1;
    int m_search_current_index = -1;

    QColor m_visual_line_mode_label_fg;
    QColor m_visual_line_mode_label_bg;
    QString m_visual_line_mode_label_padding_string;
    QString m_visual_line_mode_label_text;
    QLocale m_locale;
    QString m_file_path;
    QPalette m_message_palette;
    QTimer *m_message_timer = new QTimer(this);
};