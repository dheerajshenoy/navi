#include "InputDialog.hpp"

InputDialog::InputDialog(QWidget *parent) : QDialog(parent) {
    this->setLayout(m_layout);
    m_layout->addWidget(m_msg_label);
    m_layout->addWidget(m_line_edit);
    m_layout->addLayout(m_btn_layout);
    m_btn_layout->addWidget(m_ok_btn);
    m_btn_layout->addWidget(m_cancel_btn);

    connect(m_ok_btn, &QPushButton::clicked, this, [&]() {
    });
}

QString InputDialog::getText(const QString &title, const QString &msg,
                             bool &ok) noexcept {
    this->setWindowTitle(title);
    m_msg_label->setText(msg);
    eventLoop.exec();
}