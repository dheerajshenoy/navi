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

    connect(m_line_edit, &LineEdit::hideRequested, this, [&]() {
        this->hide();
    });
}

QString Inputbar::getInput(const QString& prompt) noexcept {
    this->show();
    m_prompt_label->setText(prompt);
    m_line_edit->setFocus();
    m_line_edit->clear();

    QString userInput;
    QEventLoop loop;

    // Capture input and exit the loop upon Enter press
    auto captureInput = [this, &userInput, &loop]() {
        userInput = m_line_edit->text();
        loop.quit();
    };

    // When the user presses Enter (returnPressed), capture the text and exit the loop
    connect(m_line_edit, &QLineEdit::returnPressed, this, captureInput);

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