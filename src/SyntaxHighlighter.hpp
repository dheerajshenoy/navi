#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTextDocument>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:

    enum class Language {
        C = 0,
        CPP,
        PYTHON,
    };

    SyntaxHighlighter(QTextDocument *parent = nullptr);

    void setLanguage(const Language &lang) noexcept;

signals:
    void languageChanged(const Language &language);

protected:
    void highlightBlock(const QString &text) override;

private:
    void initDefaultRules() noexcept;
    void setKeywords(const Language &language) noexcept;
    void load_C_keywords() noexcept;
    void load_CPP_keywords() noexcept;
    void load_PYTHON_keywords() noexcept;
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QList<HighlightingRule> highlightingRules;


    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QRegularExpression preprocessorPattern;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QStringList m_keyword_list;
    QTextCharFormat preprocessorFormat;

    Language m_language;
};