#include "Inputbar.hpp"
#include "FilePathWidget.hpp"

Inputbar::Inputbar(QWidget *parent) : QWidget(parent) {
    this->setLayout(m_layout);
    m_line_edit_completer = new QCompleter();
    m_line_edit_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_line_edit_completer->setCompletionColumn(0);
    m_line_edit->setCompleter(m_line_edit_completer);
    m_layout->addWidget(m_prompt_label);
    m_layout->addWidget(m_line_edit);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    connect(m_line_edit, &LineEdit::historyUp, this, &Inputbar::historyUp);
    connect(m_line_edit, &LineEdit::historyDown, this, &Inputbar::historyDown);
    connect(m_line_edit, &LineEdit::tabPressed, this, &Inputbar::suggestionComplete);
    connect(m_line_edit, &LineEdit::hideRequested, this, [&]() {
        this->hide();
        m_history_index = -1;
    });
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
        switch (m_current_completion_type) {
        case CompletionModelType::SEARCH:
            m_search_history_list.append(userInput);
            break;

        case CompletionModelType::COMMAND:
            m_command_history_list.append(userInput);
            break;
        }
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

void Inputbar::addCompleterModel(QAbstractItemModel *model,
                                 const CompletionModelType &type) noexcept {
    m_completer_hash.insert(type, model);
    // m_line_edit_completer->setModel(model);
}

void Inputbar::setCurrentCompletionModel(const CompletionModelType &type) noexcept {
    if (m_completer_hash.contains(type))
        m_line_edit_completer->setModel(m_completer_hash[type]);
    m_current_completion_type = type;
}

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

void Inputbar::historyUp() noexcept {
    QString historyText;

    switch (m_current_completion_type) {

    case CompletionModelType::SEARCH: {
        auto size = m_search_history_list.size();
        if (m_search_history_list.isEmpty() ||
            m_history_index >= size - 1)
            return;
        m_history_index++;
        historyText = m_search_history_list.at(size - m_history_index - 1);
        if (historyText.isEmpty() || historyText.isNull())
            return;
    } break;

    case CompletionModelType::COMMAND: {
        auto size = m_command_history_list.size();
        if (m_command_history_list.isEmpty() ||
            m_history_index >= size - 1)
            return;
        m_history_index++;
        historyText = m_command_history_list.at(size - m_history_index - 1);
        if (historyText.isEmpty() || historyText.isNull())
            return;
    } break;

    default:
        return;
    }

    m_line_edit->setText(historyText);

}

void Inputbar::historyDown() noexcept {
    QString historyText;
    switch (m_current_completion_type) {

    case CompletionModelType::SEARCH: {
        if (m_search_history_list.isEmpty() || m_history_index <= 0)
            return;
        auto size = m_command_history_list.size();
        m_history_index--;
        historyText = m_search_history_list.at(size - m_history_index - 1);
        if (historyText.isEmpty() || historyText.isNull())
            return;
    } break;

    case CompletionModelType::COMMAND: {
        if (m_command_history_list.isEmpty() || m_history_index <= 0)
            return;
        auto size = m_command_history_list.size();
        m_history_index--;
        historyText = m_command_history_list.at(size - m_history_index - 1);
        if (historyText.isEmpty() || historyText.isNull())
            return;
    } break;
    }

    m_line_edit->setText(historyText);
}
