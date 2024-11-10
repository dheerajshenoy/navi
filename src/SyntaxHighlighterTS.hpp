#pragma once

#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-c.h>
#include <tree_sitter/tree-sitter-python.h>
#include <tree_sitter/tree-sitter-markdown.h>
#include <tree_sitter/tree-sitter-lua.h>
#include <QTextFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QSyntaxHighlighter>

class SyntaxHighlighterTS : QSyntaxHighlighter {

 public:
    SyntaxHighlighterTS(QTextDocument *document);
    enum class Language {
      C = 0,
      CPP,
      PYTHON,
      MARKDOWN,
      LUA
   };

    void setLanguage(const Language &lang) noexcept;

protected:
    void highlightBlock(const QString &text) override;

private:
    void highlightNode(TSNode node, const QString &text) noexcept;
    TSParser *m_parser = nullptr;
    Language m_language;
};