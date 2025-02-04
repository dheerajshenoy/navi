#include "CustomInputDialog.hpp"

CustomInputDialog::CustomInputDialog(QWidget *parent) : QDialog(parent) {
    this->setLayout(m_layout);
    m_layout->addWidget(m_msg_label);
    m_layout->addWidget(m_line_edit);
    m_layout->addLayout(m_btn_layout);
    m_btn_layout->addWidget(m_ok_btn);
    m_btn_layout->addWidget(m_cancel_btn);


}

QString CustomInputDialog::getText(const QString &title,
                                   const QString &text,
                                   bool &ok,
                                   const QString &default_text,
                                   const QString &selection_text) noexcept {
    this->show();
    this->setWindowTitle(title);
    m_msg_label->setText(text);
    auto from = default_text.indexOf(selection_text);
    auto to = from + selection_text.length();
    m_line_edit->setText(default_text);
    m_line_edit->setSelection(from, to);

    QEventLoop loop;

    connect(m_ok_btn, &QPushButton::clicked, this, [&]() {
        m_result = CustomInputDialog::DialogCode::Accepted;
        ok = true;
        loop.quit();
    });

    connect(m_cancel_btn, &QPushButton::clicked, this, [&]() {
        m_result = CustomInputDialog::DialogCode::Rejected;
        ok = false;
        loop.quit();
    });

    connect(this, &CustomInputDialog::escapePressed, this, [&]() {
        ok = false;
        m_result = CustomInputDialog::DialogCode::Rejected;
        loop.quit();
    });

    connect(this, &CustomInputDialog::returnPressed, this, [&]() {
        ok = true;
        m_result = CustomInputDialog::DialogCode::Accepted;
        loop.quit();
    });

    // Start the event loop
    loop.exec();

    return m_line_edit->text();  // Return the captured input
}

