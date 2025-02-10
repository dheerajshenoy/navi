#pragma once

#include "pch/pch_statusbar.hpp"
#include "utils.hpp"
#include "sol/sol.hpp"
#include <qboxlayout.h>

enum class MessageType { INFO = 0, WARNING, ERROR };

class Statusbar : public QWidget {
    Q_OBJECT

public:

    Statusbar(QWidget *parent = nullptr);
    ~Statusbar();

    struct Module {
        std::string name;
        sol::table options;
    };

    void Message(const QString &message, MessageType type = MessageType::INFO,
                 int ms = 2) noexcept;

    void SetFile(const QString &filename) noexcept;
    void UpdateFile() noexcept;
    void SetFilterMode(const bool state) noexcept;
    void SetNumItems(const int &numItems) noexcept;
    void SetSearchMatchCount(const int &totalCount) noexcept;
    void SetSearchMatchIndex(const int &matchIndex) noexcept;
    void SetVisualLineMode(const bool &state) noexcept;
    void SetVisualLineModeBackground(const QString &bg) noexcept;
    void SetVisualLineModeForeground(const QString &fg) noexcept;
    void SetVisualLineModeItalic(const bool &state) noexcept;
    void SetVisualLineModeBold(const bool &state) noexcept;
    void SetVisualLineModePadding(const QString &padding) noexcept;
    void SetVisualLineModeText(const QString &text) noexcept;
    void SetMacroMode(const bool &state) noexcept;
    void SetMacroModeBackground(const QString &bg) noexcept;
    void SetMacroModeForeground(const QString &fg) noexcept;
    void SetMacroModeItalic(const bool &state) noexcept;
    void SetMacroModeBold(const bool &state) noexcept;
    void SetMacroModePadding(const QString &padding) noexcept;
    void SetMacroModeText(const QString &text) noexcept;
    void addModule(const QString &moduleName) noexcept;
    void createModule(const std::string &name, const sol::function &func,
                      const sol::table &options);
    void addModules(const QStringList &names) noexcept;
    void show() noexcept;
    void hide() noexcept;

    Statusbar::Module Lua__CreateModule(const std::string &name,
                                        const sol::table &options) noexcept;
    void Lua__AddModule(const Statusbar::Module &module) noexcept;
    void Lua__InsertModule(const Statusbar::Module &module,
                           const uint32_t &index) noexcept;
    void Lua__SetModules(const sol::table &table) noexcept;
    sol::table Lua__Get_modules() noexcept;
    void Lua__UpdateModuleText(const std::string &name,
                               const std::string &value) noexcept;

    inline void set_font_family(const QString &family) noexcept {
        QFont _font = this->font();
        _font.setFamily(family);
        setFont(_font);
    }

    inline void set_background_color(const QString &color) noexcept {
        QPalette palette;
        m_background_color = color;
        palette.setColor(QPalette::Window, QColor(m_background_color));
        this->setPalette(palette);
    }

    inline std::string get_background_color() noexcept {
        return m_background_color.toStdString();
    }

    inline std::string get_font_family() noexcept {
        return font().family().toStdString();
    }

    void set_font_size(const int &size) noexcept;

    inline int get_font_size() noexcept {
        return font().pixelSize();
    }

    inline QString get_visual_line_mode_padding() noexcept { return m_visual_line_mode_label_padding_string; }
    inline bool get_visual_line_mode_italic() noexcept { return m_visual_line_mode_italic; }
    inline bool get_visual_line_mode_bold() noexcept { return m_visual_line_mode_bold; }
    inline QString get_visual_line_mode_background() noexcept {
        return m_visual_line_mode_label_bg.name();
    }

    inline QString get_visual_line_mode_foreground() noexcept {
        return m_visual_line_mode_label_fg.name();
    }

    inline void set_warning_fg(const std::string &fg) noexcept {
        m_message_warning_fg = QColor(QString::fromStdString(fg));
    }

    inline std::string get_warning_fg() noexcept {
        return m_message_warning_fg.name().toStdString();
    }

    inline void set_info_fg(const std::string &fg) noexcept {
        m_message_info_fg = QColor(QString::fromStdString(fg));
    }

    inline std::string get_info_fg() noexcept {
        return m_message_info_fg.name().toStdString();
    }

    inline void set_error_fg(const std::string &fg) noexcept {
        m_message_error_fg = QColor(QString::fromStdString(fg));
    }

    inline std::string get_error_fg() noexcept {
        return m_message_error_fg.name().toStdString();
    }

signals:
    void visibilityChanged(const bool &state);
    void logMessage(const QString &message, const MessageType &type);

private:
    QVBoxLayout *m_vert_layout = new QVBoxLayout();
    QHBoxLayout *m_layout = new QHBoxLayout();
    QLabel *m_message_label = new QLabel();
    QLabel *m_file_name_label = new QLabel();
    QLabel *m_file_size_label = new QLabel();
    QLabel *m_file_perm_label = new QLabel();
    QLabel *m_file_modified_label = new QLabel();
    QLabel *m_filter_label = new QLabel("FILTER");
    QLabel *m_num_items_label = new QLabel();
    QLabel *m_visual_line_mode_label = new QLabel("VISUAL");
    QLabel *m_search_match_label = new QLabel();
    QLabel *m_macro_mode_label = new QLabel();

    int m_search_total_count = -1;
    int m_search_current_index = -1;

    QColor m_visual_line_mode_label_fg;
    QColor m_visual_line_mode_label_bg;
    QString m_visual_line_mode_label_padding_string;
    QString m_visual_line_mode_label_text;
    bool m_visual_line_mode_italic = false, m_visual_line_mode_bold = false;
    QString m_macro_mode_label_text;
    QColor m_macro_mode_label_fg, m_macro_mode_label_bg;
    QColor m_message_info_fg = Qt::white,
    m_message_warning_fg = Qt::yellow,
    m_message_error_fg = Qt::red;
    QString m_macro_mode_label_padding_string;
    QLocale m_locale;
    QString m_file_path;
    QPalette m_message_palette;
    QTimer *m_message_timer = new QTimer(this);
    QHash<QString, QLabel*> m_module_widget_hash;
    QString m_background_color;
    sol::table m_modules;
};
