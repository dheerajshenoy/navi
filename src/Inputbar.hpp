#pragma once

#include "pch/pch_inputbar.hpp"
#include <QEvent>
#include "CompletionPopup.hpp"


class Inputbar : public QWidget {
    Q_OBJECT
public:
    Inputbar(QWidget *parent = nullptr);
    ~Inputbar();

    QString getInput(const QString &prompt, const QString &defaultValue = 0,
                     const QString &selectionString = 0) noexcept;

    enum CompletionModelType {
        COMMAND = 0,
        LUA_FUNCTIONS
    };
    void setFontItalic(const bool &state) noexcept;
    void setFontBold(const bool &state) noexcept;
    void setForeground(const QString &foreground) noexcept;
    void setForeground(const std::string &foreground) noexcept;
    void setBackground(const QString &background) noexcept;
    void setBackground(const std::string &background) noexcept;
    void setFontFamily(const QString &family) noexcept;
    void setFontFamily(const std::string &family) noexcept;
    // returns the QLineEdit
    LineEdit *lineEdit() noexcept { return m_line_edit; }

    // returns the font family
    inline std::string get_font() noexcept { return font().family().toStdString(); }

    // sets the font
    void set_font_size(const int &size = 14) noexcept;

    // returns the font pixel size
    inline int get_font_size() noexcept { return font().pixelSize(); }

    CompletionPopup* completionPopup() noexcept { return m_completion; }

    // return the background color
    inline std::string Get_background_color() noexcept { return m_background_color.toStdString(); }

    // return the foreground color
    inline std::string Get_foreground_color() noexcept {
      return m_foreground_color.toStdString();
    }

    void enableCompletion() noexcept;
    void disableCompletion() noexcept;

signals:
    void lineEditTextChanged(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    void suggestionComplete() noexcept;
    QHBoxLayout *m_layout = new QHBoxLayout();
    QLabel *m_prompt_label = new QLabel();
    LineEdit *m_line_edit = new LineEdit();
    CompletionPopup *m_completion = new CompletionPopup(m_line_edit);
    QStringList m_lua_function_stringlist = {};
    QString m_background_color, m_foreground_color;
};
