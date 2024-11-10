#pragma once

#include <QTextEdit>
#include "SyntaxHighlighter.hpp"

class TextEdit : public QTextEdit {

public:
    TextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

    inline void setSyntaxHighlighting(const bool &state) noexcept {
        if (state) {
            if (!m_syntax_highlighter)
                m_syntax_highlighter = new SyntaxHighlighter(this->document());
        } else {
            delete m_syntax_highlighter;
            m_syntax_highlighter = nullptr;
        }
    }

    inline void
    setLanguage(const SyntaxHighlighter::Language &language) noexcept {
        if (!m_syntax_highlighter)
            m_syntax_highlighter = new SyntaxHighlighter(this->document());
        m_syntax_highlighter->setLanguage(language);
    }

private:
    SyntaxHighlighter *m_syntax_highlighter = nullptr;
};
