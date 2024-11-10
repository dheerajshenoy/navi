#include "SyntaxHighlighterTS.hpp"
#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-c.h>

SyntaxHighlighterTS::SyntaxHighlighterTS(QTextDocument *document)
: QSyntaxHighlighter(document) {
    m_parser = ts_parser_new();
}

void SyntaxHighlighterTS::setLanguage(const Language &lang) noexcept {
    m_language = lang;

  switch (lang) {

  case Language::C:
      ts_parser_set_language(m_parser, tree_sitter_c());
      break;

  case Language::CPP:
      break;

  case Language::PYTHON:
      ts_parser_set_language(m_parser, tree_sitter_python());
      break;

  case Language::MARKDOWN:
      ts_parser_set_language(m_parser, tree_sitter_markdown());
      break;

  case Language::LUA:
      ts_parser_set_language(m_parser, tree_sitter_lua());
      break;
  }
}

void SyntaxHighlighterTS::highlightBlock(const QString &text) {
    auto tmp = text.toStdString();
    auto code = tmp.c_str();
    int length;
    const char *queryString;
    TSQuery *query = nullptr;
    auto errorType = TSQueryError::TSQueryErrorNone;
    switch (m_language) {
    case Language::C:
      queryString = "(comment) @comment"
                    "(_ (string_literal) @string)"
                    "(preproc_include (system_lib_string) @include_lib)"
                    "(_ (identifier) @identifier)"
                    "(_ (primitive_type) @primitive_type)"
                    "(preproc_include) @directive"
                    "(preproc_if) @directive"
                    "(preproc_ifdef) @directive"
                    "(preproc_else) @directive"
                    "(preproc_elif) @directive"
                    "(preproc_directive) @directive"
                    "(_ (return_statement) @primitive_type)";
        length = strlen(queryString);
        query = ts_query_new(tree_sitter_c(), queryString, length, 0, &errorType);
        break;

    // case Language::CPP:
    //     break;

    case Language::PYTHON:
      queryString = "(_ (identifier) @identifier)"
                    "(_ (type) @primitive_type)"
                    "(_ (string_content) @string)"
                    "(function_definition (identifier) @function.identifier)"
                    "(_ (return_statement) @primitive_type)";

        length = strlen(queryString);
        query = ts_query_new(tree_sitter_python(), queryString, length, 0, &errorType);
        break;

    // case Language::MARKDOWN:
    //     query = ts_query_new(tree_sitter_markdown(), queryString, length, 0, &errorType);
    //     break;

    // case Language::LUA:
    //     query = ts_query_new(tree_sitter_lua(), queryString, length, 0, &errorType);
    //     break;
    }

    if (!query)
        return;
    // // Parse the source code into a Tree-sitter syntax tree
    TSTree *tree = ts_parser_parse_string(m_parser, nullptr, code, strlen(code));
    TSNode root_node = ts_tree_root_node(tree);

    TSQueryCursor *cursor = ts_query_cursor_new();
    ts_query_cursor_exec(cursor, query, root_node);
    TSQueryMatch match;
    // // Loop through matches and highlight keywords
    while (ts_query_cursor_next_match(cursor, &match)) {
        for (uint32_t i = 0; i < match.capture_count; ++i) {
            TSNode node = match.captures->node;

            // Get the position of the keyword in the source code
            uint32_t start_byte = ts_node_start_byte(node);
            uint32_t end_byte = ts_node_end_byte(node);
            uint32_t length;
            const char *captureName = ts_query_capture_name_for_id(
                                                                   query, match.captures[i].index, &length);
            QTextCharFormat format;
            if (strcmp(captureName, "comment") == 0) {
                format.setForeground(Qt::lightGray); // Set color for highlighting
                format.setFontItalic(true);
            }

            else if (strcmp(captureName, "primitive_type") == 0) {
                format.setForeground(Qt::cyan); // Set color for highlighting
            }

            else if (strcmp(captureName, "string") == 0) {
                format.setForeground(QColor::fromString("#FF5000"));
            }

            else if (strcmp(captureName, "include_lib") == 0) {
                format.setForeground(QColor::fromString("#FF5000"));
            }

            else if (strcmp(captureName, "identifier") == 0) {
                format.setForeground(QColor::fromString("#FF2131"));
            }

            else if (strcmp(captureName, "directive") == 0) {
                format.setForeground(QColor::fromString("#FF2453"));
            }

            // Highlight the keyword in the QTextBlock
            int start = static_cast<int>(start_byte);
            int end = static_cast<int>(end_byte);
            setFormat(start, end - start, format);
        }
    }

    ts_query_cursor_delete(cursor);
    ts_query_delete(query);
    ts_tree_delete(tree);
}
