#pragma once

#include <QTextEdit>
#include <QPainter>
#include <QTextCursor>
#include <QString>
#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-python.h>
#include <tree_sitter/tree-sitter-lua.h>
#include <tree_sitter/tree-sitter-bash.h>
#include <tree_sitter/tree-sitter-c.h>
#include <tree_sitter/tree-sitter-markdown.h>
#include <tree_sitter/tree-sitter-query.h>

class TreeSitterTextEdit : public QTextEdit {
    Q_OBJECT

public:
    explicit TreeSitterTextEdit(QWidget *parent = nullptr);
    ~TreeSitterTextEdit();

    void setText(const QString &text) noexcept;

private:
    TSParser *parser;
    TSTree *tree;

    void highlightSyntax() noexcept;
    QColor getColorForNode(const TSNode node) noexcept;
};