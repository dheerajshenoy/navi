#include "AboutWidget.hpp"

AboutWidget::AboutWidget(QWidget *parent) : QDialog(parent) {

    this->setLayout(m_layout);

    m_layout->addWidget(m_main_label);
    m_layout->addWidget(m_sub_label);

    QFont main_label_font = m_main_label->font();
    main_label_font.setPixelSize(40);
    m_main_label->setFont(main_label_font);
    m_main_label->setFont(main_label_font);

    m_sub_label->setText("Modern fast file manager for the impatient");

    m_layout->addWidget(m_github_btn);
    m_layout->addWidget(m_linkedin_btn);

    this->exec();
}
