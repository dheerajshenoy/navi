#include "Statusbar.hpp"
#include <qnamespace.h>

Statusbar::Statusbar(QWidget *parent) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_layout->setContentsMargins(10, 0, 10, 10);
    m_vert_layout->setContentsMargins(10, 0, 10, 0);

    m_message_label->hide();
    m_vert_layout->addLayout(m_layout);
    m_layout->addWidget(m_macro_mode_label);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_visual_line_mode_label);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_file_name_label);
    m_layout->addStretch();
    m_layout->addWidget(m_filter_label);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_search_match_label);
    m_layout->addSpacing(10);
    m_layout->addWidget(new QLabel("Items: "));
    m_layout->addWidget(m_num_items_label);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_file_size_label);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_file_modified_label);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_file_perm_label);

    m_visual_line_mode_label->setHidden(true);
    m_macro_mode_label->setHidden(true);

    m_vert_layout->addWidget(m_message_label);
    m_filter_label->setHidden(true);
    m_message_timer->setSingleShot(true);
    connect(m_message_timer, &QTimer::timeout, this, [&]() {
        m_message_label->hide();
    });

    m_message_palette = m_message_label->palette();
    this->setLayout(m_vert_layout);
}

void Statusbar::Message(const QString &message, const MessageType type,
                        int ms) noexcept {

    switch (type) {
    case MessageType::INFO:
        m_message_palette.setColor(m_message_label->foregroundRole(), Qt::white);
        break;

    case MessageType::WARNING:
        m_message_palette.setColor(m_message_label->foregroundRole(), Qt::yellow);
        break;

    case MessageType::ERROR:
        m_message_palette.setColor(m_message_label->foregroundRole(), Qt::red);
        break;
    }

    m_message_label->setPalette(m_message_palette);
    m_message_label->setText(message);

    m_message_label->show();

    // QTimer::singleShot(ms * 1000, [&]() { m_message_label->hide(); });
    m_message_timer->setInterval(ms * 1000);
    m_message_timer->start();

    emit logMessage(message, type);
}

// Set the number of items in the directory
void Statusbar::SetNumItems(const int &numItems) noexcept {
    m_num_items_label->setText(QString::number(numItems));
}

void Statusbar::SetFile(const QString &file) noexcept {
    m_file_path = file;
    QFileInfo fileInfo(file);
    m_file_name_label->setText(fileInfo.fileName());
    m_file_perm_label->setText(utils::getPermString(fileInfo));
    m_file_size_label->setText(m_locale.formattedDataSize(fileInfo.size()));
    m_file_modified_label->setText(fileInfo.lastModified().toString());
}

void Statusbar::UpdateFile() noexcept {
    QFileInfo fileInfo(m_file_path);
    m_file_name_label->setText(fileInfo.fileName());
    m_file_perm_label->setText(utils::getPermString(fileInfo));
    m_file_size_label->setText(m_locale.formattedDataSize(fileInfo.size()));
    m_file_modified_label->setText(fileInfo.lastModified().toString());
}

void Statusbar::SetFilterMode(const bool state) noexcept {
    if (state)
        m_filter_label->show();
    else
        m_filter_label->hide();
}

Statusbar::~Statusbar() {}

void Statusbar::SetVisualLineMode(const bool &state) noexcept {
    if (state)
        m_visual_line_mode_label->show();
    else
        m_visual_line_mode_label->hide();
}

void Statusbar::SetSearchMatchIndex(const int &searchIndex) noexcept {
    m_search_current_index = searchIndex + 1;
    m_search_match_label->setText(QString("[%1/%2]").arg(m_search_current_index).arg(m_search_total_count));

}

void Statusbar::SetSearchMatchCount(const int &totalCount) noexcept {
    m_search_total_count = totalCount;
}

void Statusbar::SetMacroMode(const bool &state) noexcept {
    m_macro_mode_label->setVisible(state);
}


void Statusbar::SetVisualLineModeBackground(const QString &bg) noexcept {
    m_visual_line_mode_label_bg = bg;
    m_visual_line_mode_label->setStyleSheet(QString("background: %1; foreground: %2; padding: 2px; ")
                .arg(m_visual_line_mode_label_bg.name())
                .arg(m_visual_line_mode_label_fg.name())
                .arg(m_visual_line_mode_label_padding_string));
}

void Statusbar::SetVisualLineModeForeground(const QString &fg) noexcept {
    m_visual_line_mode_label_fg = fg;
    m_visual_line_mode_label->setStyleSheet(QString("background: %1; foreground: %2; padding: 2px; ")
                .arg(m_visual_line_mode_label_bg.name())
                .arg(m_visual_line_mode_label_fg.name())
                .arg(m_visual_line_mode_label_padding_string));
}

void Statusbar::SetVisualLineModeItalic(const bool &state) noexcept {
    QFont font = m_visual_line_mode_label->font();
    font.setItalic(state);
    m_visual_line_mode_label->setFont(font);
}

void Statusbar::SetVisualLineModeBold(const bool &state) noexcept {
    QFont font = m_visual_line_mode_label->font();
    font.setBold(state);
    m_visual_line_mode_label->setFont(font);
}

void Statusbar::SetVisualLineModePadding(const QString &padding) noexcept {
    m_visual_line_mode_label_padding_string = padding;
    m_visual_line_mode_label->setStyleSheet(QString("background: %1; foreground: %2; padding: %3; ")
                .arg(m_visual_line_mode_label_bg.name())
                .arg(m_visual_line_mode_label_fg.name())
                .arg(m_visual_line_mode_label_padding_string));
}

void Statusbar::SetVisualLineModeText(const QString &text) noexcept {
    m_visual_line_mode_label_text = text;
    m_visual_line_mode_label->setText(text);
}



void Statusbar::SetMacroModeBackground(const QString &bg) noexcept {
    m_macro_mode_label_bg = bg;
    m_macro_mode_label->setStyleSheet(QString("background: %1; foreground: %2; padding: 2px; ")
                .arg(m_macro_mode_label_bg.name())
                .arg(m_macro_mode_label_fg.name())
                .arg(m_macro_mode_label_padding_string));
}

void Statusbar::SetMacroModeForeground(const QString &fg) noexcept {
    m_macro_mode_label_fg = fg;
    m_macro_mode_label->setStyleSheet(QString("background: %1; foreground: %2; padding: 2px; ")
                .arg(m_macro_mode_label_bg.name())
                .arg(m_macro_mode_label_fg.name())
                .arg(m_macro_mode_label_padding_string));
}

void Statusbar::SetMacroModeItalic(const bool &state) noexcept {
    QFont font = m_macro_mode_label->font();
    font.setItalic(state);
    m_macro_mode_label->setFont(font);
}

void Statusbar::SetMacroModeBold(const bool &state) noexcept {
    QFont font = m_macro_mode_label->font();
    font.setBold(state);
    m_macro_mode_label->setFont(font);
}

void Statusbar::SetMacroModePadding(const QString &padding) noexcept {
    m_macro_mode_label_padding_string = padding;
    m_macro_mode_label->setStyleSheet(QString("background: %1; foreground: %2; padding: %3; ")
                .arg(m_macro_mode_label_bg.name())
                .arg(m_macro_mode_label_fg.name())
                .arg(m_macro_mode_label_padding_string));
}

void Statusbar::SetMacroModeText(const QString &text) noexcept {
    m_macro_mode_label_text = text;
    m_macro_mode_label->setText(text);
}

void Statusbar::show() noexcept {
    emit visibilityChanged(true);
    QWidget::show();
}

void Statusbar::hide() noexcept {
    emit visibilityChanged(false);
    QWidget::hide();
}