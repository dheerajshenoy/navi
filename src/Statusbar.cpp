#include "Statusbar.hpp"
#include <qnamespace.h>

Statusbar::Statusbar(QWidget *parent) : QStatusBar(parent) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->setSizeGripEnabled(false);

    this->addPermanentWidget(m_message_label);
    m_message_label->hide();

    m_visual_line_mode_label->setHidden(true);
    m_macro_mode_label->setHidden(true);

    m_filter_label->setHidden(true);
    m_message_timer->setSingleShot(true);
    connect(m_message_timer, &QTimer::timeout, this, [&]() {
        m_message_label->hide();
    });

    m_message_palette = m_message_label->palette();
}

void Statusbar::Message(const QString &message, const MessageType type,
                        int ms) noexcept {
    if (!m_message_label->parent())
        return;

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
    if (!m_filter_label->parent())
        return;

    if (state)
        m_filter_label->show();
    else
        m_filter_label->hide();
}

Statusbar::~Statusbar() {}

void Statusbar::SetVisualLineMode(const bool &state) noexcept {
    if (!m_visual_line_mode_label->parent())
        return;

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
    m_visual_line_mode_italic = state;
    QFont font = m_visual_line_mode_label->font();
    font.setItalic(state);
    m_visual_line_mode_label->setFont(font);
}

void Statusbar::SetVisualLineModeBold(const bool &state) noexcept {
    m_visual_line_mode_bold = state;
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

void Statusbar::addModule(const QString &name) noexcept {
    if (name == "name") {
        addPermanentWidget(m_file_name_label);
    }

    else if (name == "stretch") {
        addPermanentWidget(new QLabel(), 1);
    }

    else if (name == "size") {
        addPermanentWidget(m_file_size_label);
    }

    else if (name == "permission") {
        addPermanentWidget(m_file_perm_label);
    }

    else if (name == "count") {
        addPermanentWidget(new QLabel("Items: "));
        addPermanentWidget(m_num_items_label);
    }

    else if (name == "modified_date") {
        addPermanentWidget(m_file_modified_label);
    }

    else if (name == "visual_line") {
        addPermanentWidget(m_visual_line_mode_label);
    }

    else if (name == "macro") {
        addPermanentWidget(m_macro_mode_label);
    }

    else if (name == "filter") {
        addPermanentWidget(m_filter_label);
    }

    else if (name == "search") {
        addPermanentWidget(m_search_match_label);
    } else if (m_module_widget_hash.contains(name)) {
        auto widget = m_module_widget_hash[name];
        if (widget) {
            addPermanentWidget(widget);
        }
    }

}

Statusbar::Module Statusbar::Lua__CreateModule(const std::string &name,
                                     const sol::table &options) noexcept {
    Statusbar::Module mod = Module();
    mod.name = name;
    mod.options = options;
    return mod;
}

void Statusbar::Lua__AddModule(const Statusbar::Module &module) noexcept {
    auto options = module.options;
    auto text = options["text"].get_or<std::string>("");
    auto italic = options["italic"].get_or(false);
    auto bold = options["bold"].get_or(false);
    auto bg = options["background"].get_or(QColor().name().toStdString());
    auto fg = options["foreground"].get_or(QColor().name().toStdString());
    auto visible = options["visible"].get_or(true);

    QLabel *label = new QLabel();

    if (!text.empty())
        label->setText(QString::fromStdString(text));

    QFont font = label->font();
    font.setItalic(italic);
    font.setBold(bold);
    label->setFont(font);
    label->setStyleSheet(QString("background: %1; color: %2; padding: 2px;")
            .arg(QString::fromStdString(bg))
            .arg(QString::fromStdString(fg)));

    addPermanentWidget(label);

    if (!visible)
        label->setHidden(true);

    m_module_widget_hash.insert(QString::fromStdString(module.name), label);
}

void Statusbar::Lua__InsertModule(const Statusbar::Module &module,
                                  const uint32_t &index) noexcept {
    auto options = module.options;
    auto text = options["text"].get_or<std::string>("");
    auto italic = options["italic"].get_or(false);
    auto bold = options["bold"].get_or(false);
    auto bg = options["background"].get_or<std::string>("");
    auto fg = options["foreground"].get_or<std::string>("");
    auto hidden = options["visible"].get_or(false);

    QLabel *label = new QLabel();

    if (!text.empty())
        label->setText(QString::fromStdString(text));

    QFont font = label->font();
    font.setItalic(italic);
    font.setBold(bold);
    label->setFont(font);
    label->setStyleSheet(QString("background: %1; color: %2; padding: 2px;")
            .arg(QString::fromStdString(bg))
            .arg(QString::fromStdString(fg)));

    insertWidget(index, label);
    label->setVisible(!hidden);

    m_module_widget_hash.insert(QString::fromStdString(module.name), label);
}


void Statusbar::Lua__UpdateModuleText(const std::string &name,
                                      const std::string &value) noexcept {
    auto *widget = m_module_widget_hash[QString::fromStdString(name)];
    if (widget)
        widget->setText(QString::fromStdString(value));
}

void Statusbar::Lua__SetModules(const sol::table &table) noexcept {
    if (table.valid()) {

        // If widgets are already present, remove them (not delete)
        /*while (QLayoutItem *item = layout()->takeAt(0)) {*/
        /*    if (QWidget *widget = item->widget()) {*/
        /*        widget->setParent(nullptr);*/
        /*        widget->deleteLater();*/
        /*    }*/
        /*    delete item;*/
        /*}*/
        // TODO:

        // Add the modules
        for (const auto &module : table) {
            if (module.second.is<std::string>())
                addModule(QString::fromStdString(module.second.as<std::string>()));
            else if (module.second.is<Statusbar::Module>()) {
                Lua__AddModule(module.second.as<Statusbar::Module>());
            }
        }

        m_modules = table;
    }
}

sol::table Statusbar::Lua__Get_modules() noexcept {
    return m_modules;
}

void Statusbar::set_font_size(const int &size) noexcept {
    QFont _font = this->font();
    _font.setPixelSize(size);
    setFont(_font);
}
