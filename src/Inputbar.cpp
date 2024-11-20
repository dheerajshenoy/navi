#include "Inputbar.hpp"
#include "FilePathWidget.hpp"
#include <qnamespace.h>

Inputbar::Inputbar(QWidget *parent) : QWidget(parent) {
    this->setLayout(m_layout);

    m_filter_model = new OrderlessFilterModel(this);
    m_line_edit_completer = new InputbarCompleter(m_filter_model, this);
    m_line_edit_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_line_edit_completer->setFilterMode(Qt::MatchFlag::MatchContains);
    m_line_edit_completer->setCompletionPrefix(" ");
    m_line_edit_completer->setCompletionColumn(0);
    m_filter_model->setSourceModel(m_completer_model);
    m_line_edit->setCompleter(m_line_edit_completer);
    m_layout->addWidget(m_prompt_label);
    m_layout->addWidget(m_line_edit);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    connect(m_line_edit, &LineEdit::tabPressed, this, &Inputbar::suggestionComplete);
    connect(m_line_edit, &LineEdit::hideRequested, this, [&]() {
      this->hide();
    });

    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

void Inputbar::suggestionComplete() noexcept {
    auto index = m_line_edit_completer->currentIndex();
    m_line_edit_completer->popup()->setCurrentIndex(index);
    auto start = m_line_edit_completer->currentRow();
    if (!m_line_edit_completer->setCurrentRow(start++))
        m_line_edit_completer->setCurrentRow(0);
}

QString Inputbar::getInput(const QString &prompt,
                           const QString &defaultValue, const QString &selectionString) noexcept {
    this->show();
    m_prompt_label->setText(prompt);
    m_line_edit->setFocus();
    m_line_edit->clear();

    if (!(defaultValue.isNull() && defaultValue.isEmpty())) {
        m_line_edit->setText(defaultValue);
        m_line_edit->setSelection(0, defaultValue.indexOf(selectionString) + selectionString.size());
    }

    QString userInput;
    QEventLoop loop;

    // Capture input and exit the loop upon Enter press
    auto captureInput = [this, &userInput, &loop]() {
        userInput = m_line_edit->text();
        loop.quit();
    };

    // When the user presses Enter (returnPressed), capture the text and exit the loop
    connect(m_line_edit, &LineEdit::returnPressed, this, captureInput);

    // Start the event loop
    loop.exec();

    // Disconnect, otherwise causes crash
    disconnect(m_line_edit, &QLineEdit::returnPressed, 0, 0);

    this->hide();
    return userInput;  // Return the captured input
}

Inputbar::~Inputbar() {}

void Inputbar::enableCommandCompletions() noexcept {
    m_line_edit->setCompleter(m_line_edit_completer);
}

void Inputbar::disableCommandCompletions() noexcept {
    m_line_edit->setCompleter(nullptr);
}

void Inputbar::setBackground(const QString &background) noexcept {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(background));
    m_line_edit->setPalette(palette);
    // TODO: Unable to apply this
}

void Inputbar::setForeground(const QString &foreground) noexcept {
    QPalette palette;
    palette.setColor(QPalette::Text, QColor(foreground));
    m_line_edit->setPalette(palette);
}

void Inputbar::setFontFamily(const QString &family) noexcept {
    QFont font = m_line_edit->font();
    font.setFamily(family);
    m_line_edit->setFont(font);
}

void Inputbar::addCompletionStringList(const CompletionModelType &type,
                                  const QStringList &list) noexcept {
    m_completion_list_hash.insert(type, list);
}

void Inputbar::currentCompletionStringList(const CompletionModelType &type) noexcept {
    if (m_completion_list_hash.contains(type))
        m_completer_model->setStringList(m_completion_list_hash[type]);
    else
        m_completer_model->setStringList({});
}