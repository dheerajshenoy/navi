#include "CustomInputDialog.hpp"

CustomInputDialog::CustomInputDialog(QWidget *parent) : QDialog(parent) {
    this->setLayout(m_layout);
    m_layout->addWidget(m_msg_label);
    m_layout->addWidget(m_line_edit);
    m_layout->addLayout(m_btn_layout);
    m_btn_layout->addWidget(m_ok_btn);
    m_btn_layout->addWidget(m_cancel_btn);

    connect(m_ok_btn, &QPushButton::clicked, [&]() {
        m_result = CustomInputDialog::DialogCode::Accepted;
        eventLoop.exit();
    });

    connect(m_cancel_btn, &QPushButton::clicked, [&]() {
        m_result = CustomInputDialog::DialogCode::Rejected;
        this->close();
        eventLoop.exit();
    });
}

QString CustomInputDialog::getText(const QString &title,
                             const QString &text,
                             bool &ok,
                             const QString &selection_text) noexcept {
    this->setWindowTitle(title);
    m_msg_label->setText(text);
    eventLoop.exec();

    return m_line_edit->text();
}

