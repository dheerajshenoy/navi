#include "Inputbar.hpp"

Inputbar::Inputbar(QWidget *parent) : QWidget(parent) {
    this->setLayout(m_layout);
    m_line_edit_completer = new QCompleter();
    m_line_edit_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_line_edit_completer->setCompletionColumn(0);
    m_line_edit->setCompleter(m_line_edit_completer);
    m_layout->addWidget(m_prompt_label);
    m_layout->addWidget(m_line_edit);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    connect(m_line_edit, &LineEdit::tabPressed, this, &Inputbar::suggestionComplete);

    connect(m_line_edit, &LineEdit::hideRequested, this, [&]() {
        this->hide();
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
                           const QString &defaultValue) noexcept {
    this->show();
    m_prompt_label->setText(prompt);
    m_line_edit->setFocus();
    m_line_edit->clear();

    if (!(defaultValue.isNull() && defaultValue.isEmpty())) {
        m_line_edit->setText(defaultValue);
        int index = -1;
        if (defaultValue.contains("."))
            index = defaultValue.indexOf(".");
        else
            index = defaultValue.size();

        m_line_edit->setSelection(0, index);
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

void Inputbar::setCompleterModel(QAbstractItemModel *model) noexcept {
    m_line_edit_completer->setModel(model);
}