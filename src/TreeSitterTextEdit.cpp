#include "TreeSitterTextEdit.hpp"

TreeSitterTextEdit::TreeSitterTextEdit(QWidget *parent)
: QTextEdit(parent), parser(ts_parser_new()), tree(nullptr) {
    // Initialize Tree-sitter with a specific language

    ts_parser_set_language(
                           parser, tree_sitter_python()); // Replace with your actual language
}

TreeSitterTextEdit::~TreeSitterTextEdit() {
    ts_tree_delete(tree);
    ts_parser_delete(parser);
}

void TreeSitterTextEdit::setText(const QString &text) noexcept {
    QTextEdit::setText(text);
    // highlightSyntax();
}

void TreeSitterTextEdit::highlightSyntax() noexcept {
    QString text = toPlainText();
    ts_tree_delete(tree);
    tree = ts_parser_parse_string(parser, nullptr, text.toUtf8().constData(), text.size());

    const TSNode rootNode = ts_tree_root_node(tree);

    // Get the number of child nodes
    uint32_t childCount = ts_node_child_count(rootNode);

    // Iterate over child nodes
    for (uint32_t i = 0; i < childCount; i++) {
        const TSNode childNode = ts_node_child(rootNode, i);

        // Get the start and end byte positions of the child node
        uint32_t startByte = ts_node_start_byte(childNode);
        uint32_t endByte = ts_node_end_byte(childNode);

        // Convert byte positions to character indices
        QTextCursor cursor(this->textCursor());
        cursor.setPosition(startByte);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, endByte - startByte);

        // Determine the highlighting color based on the node type
        QColor color = getColorForNode(childNode);

        QTextCharFormat format;
        format.setBackground(color);
        cursor.mergeCharFormat(format);

        // // Apply highlighting by setting the format
        // QPalette palette = this->palette();
        // palette.setColor(QPalette::Highlight, color);

        // Paint the highlighted area
        // this->setTextCursor(cursor);
        // this->setTextColor(color);
    }
}

QColor TreeSitterTextEdit::getColorForNode(const TSNode node) noexcept {
    // Example: Return different colors based on the node type
    const char *type = ts_node_type(node);

    qDebug() << type;

    if (strcmp(type, "import_statement")) {
        return QColor(100, 0, 0);
    }

    return QColor(0, 0, 0, 0); // No color for other nodes
}
