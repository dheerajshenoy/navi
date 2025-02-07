#include "Inputbar.hpp"
#include "FilePathWidget.hpp"

Inputbar::Inputbar(QWidget *parent) : QWidget(parent) {
    this->setLayout(m_layout);

    m_layout->setContentsMargins(0, 0, 0, 0);

    m_layout->addWidget(m_prompt_label);
    m_layout->addWidget(m_line_edit);

    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

// Enables completions (if it exists)
void Inputbar::enableCompletion() noexcept {

  connect(m_line_edit, &LineEdit::tabPressed, m_completion,
          &CompletionPopup::showPopup);

  // connect(m_line_edit, &LineEdit::textChanged, m_completion,
  //         &CompletionPopup::updateCompletions);

  connect(m_line_edit, &LineEdit::hideRequested, m_line_edit, &LineEdit::hide);
    
}

// Disables completions (if it exists)
void Inputbar::disableCompletion() noexcept {
  disconnect(m_line_edit, &LineEdit::tabPressed, m_completion,
          &CompletionPopup::showPopup);

  disconnect(m_line_edit, &LineEdit::textChanged, m_completion,
          &CompletionPopup::updateCompletions);

  disconnect(m_line_edit, &LineEdit::hideRequested, m_line_edit, &LineEdit::hide);
}

QString Inputbar::getInput(const QString &prompt, const QString &defaultValue,
                           const QString &selectionString) noexcept {
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

void Inputbar::setBackground(const QString &background) noexcept {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(background));
    m_line_edit->setPalette(palette);
    m_background_color = background;
    // TODO: Unable to apply this
}

void Inputbar::setBackground(const std::string &background) noexcept {
    QPalette palette;
    m_background_color = QString::fromStdString(background);
    palette.setColor(QPalette::Window, QColor(m_background_color));
    m_line_edit->setPalette(palette);
    // TODO: Unable to apply this
}

void Inputbar::setForeground(const QString &foreground) noexcept {
    QPalette palette;
    palette.setColor(QPalette::Text, QColor(foreground));
    m_line_edit->setPalette(palette);
    m_foreground_color = foreground;
}

void Inputbar::setForeground(const std::string &foreground) noexcept {
    QPalette palette;
    m_foreground_color = QString::fromStdString(foreground);
    palette.setColor(QPalette::Text, QColor(m_foreground_color));
    m_line_edit->setPalette(palette);
}

void Inputbar::setFontFamily(const QString &family) noexcept {
    QFont font = m_line_edit->font();
    font.setFamily(family);
    m_line_edit->setFont(font);
}

void Inputbar::setFontFamily(const std::string &family) noexcept {
    QFont font = m_line_edit->font();
    font.setFamily(QString::fromStdString(family));
    m_line_edit->setFont(font);
}

void Inputbar::set_font_size(const int &size) noexcept {
    QFont _font = font();
    _font.setPixelSize(size);
    this->setFont(_font);
}

void Inputbar::keyPressEvent(QKeyEvent *e) {
  switch (e->key()) {
  case Qt::Key_Escape:
      emit m_line_edit->hideRequested();
    break;
  }

  QWidget::keyPressEvent(e);
}
