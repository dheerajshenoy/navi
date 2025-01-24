#pragma once

#include <QTextEdit>

class TextEdit : public QTextEdit {

public:
    TextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

private:
    // SyntaxHighlighter *m_syntax_highlighter = nullptr;
};
