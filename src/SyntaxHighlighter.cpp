#include "SyntaxHighlighter.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    initDefaultRules();
}

void SyntaxHighlighter::initDefaultRules() noexcept {

    HighlightingRule rule;

    keywordFormat.setForeground(Qt::gray);
    keywordFormat.setFontWeight(QFont::Bold);

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor::fromString("#FF5000"));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);
    multiLineCommentFormat.setFontItalic(true);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));

    preprocessorFormat.setForeground(QColor::fromString("#ff80f4"));
}

void SyntaxHighlighter::setKeywords(const Language &language) noexcept {
    highlightingRules.clear();
    initDefaultRules();
    switch(language) {
    case Language::C:
        load_C_keywords();
        break;

    case Language::CPP:
        load_CPP_keywords();
        break;

    case Language::PYTHON:
        load_PYTHON_keywords();
        break;

    default:
        break;

    }

    // Create a rule for each keyword
    for (const QString &keyword : m_keyword_list) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

}

void SyntaxHighlighter::load_C_keywords() noexcept {
    m_keyword_list = {
        "auto", "break", "case", "char", "const", "continue",
        "default", "do", "double", "else", "enum", "extern",
        "float", "for", "goto", "if", "inline", "int",
        "long", "register", "restrict", "return", "short", "signed",
        "sizeof", "static", "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof",
        "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn",
        "_Static_assert", "_Thread_local"
    };


    HighlightingRule rule;

    rule.pattern = QRegularExpression(QStringLiteral("^#\\w+"));
    rule.format = preprocessorFormat;
    highlightingRules.append(rule);
}

void SyntaxHighlighter::load_CPP_keywords() noexcept {
    m_keyword_list = {
        "\\#include", "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit",
        "atomic_noexcept", "auto", "bitand", "bitor", "bool", "break", "case", "catch",
        "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const",
        "consteval", "constexpr", "const_cast", "continue", "co_await", "co_return",
        "co_yield", "decltype", "default", "delete", "do", "double", "dynamic_cast",
        "else", "enum", "explicit", "export", "extern", "false", "float", "for", "friend",
        "goto", "if", "inline", "int", "long", "mutable", "namespace", "new", "noexcept",
        "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected",
        "public", "reflexpr", "register", "reinterpret_cast", "requires", "return", "short",
        "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch",
        "synchronized", "template", "this", "thread_local", "throw", "true", "try", "typedef",
        "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile",
        "wchar_t", "while", "xor", "xor_eq"
    };

    HighlightingRule rule;

    rule.pattern = QRegularExpression(QStringLiteral("^#\\w+"));
    rule.format = preprocessorFormat;
    highlightingRules.append(rule);
}

void SyntaxHighlighter::load_PYTHON_keywords() noexcept {

    m_keyword_list = {
        "False", "None", "True", "and", "as", "assert", "async", "await", "break",
        "class", "continue", "def", "del", "elif", "else", "except", "finally",
        "for", "from", "global", "if", "import", "in", "is", "lambda", "nonlocal",
        "not", "or", "pass", "raise", "return", "try", "while", "with", "yield"
    };
}

void SyntaxHighlighter::setLanguage(const Language &language) noexcept {

    m_language = language;
    setKeywords(language);
    emit languageChanged(language);
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}