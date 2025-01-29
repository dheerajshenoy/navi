#include "AboutWidget.hpp"

AboutWidget::AboutWidget(QWidget *parent) : QDialog(parent) {

    this->setLayout(m_layout);

    m_link_layout->addWidget(m_github_btn);
    m_link_layout->addWidget(m_linkedin_btn);
    m_link_layout->addStretch(1);
    m_docs_label->setTextFormat(Qt::RichText);
    m_docs_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_docs_label->setOpenExternalLinks(true);
    m_docs_label->setText("See docs <a href='https://navi.readthedocs.io'>here</a>");
    m_link_layout->addWidget(m_close_btn);

    connect(m_close_btn, &QPushButton::clicked, this, [this]() { this->close(); });

    m_version_label->setText(QString("Current Version: %1").arg(dynamic_cast<Navi*>(parent)->version()));

    m_layout->addWidget(m_main_label);
    m_layout->addWidget(m_sub_label);
    m_layout->addWidget(m_version_label);
    m_layout->addWidget(m_docs_label);

    QFont main_label_font = m_main_label->font();
    main_label_font.setPixelSize(40);
    m_main_label->setFont(main_label_font);
    m_main_label->setFont(main_label_font);

    m_sub_label->setText("Modern fast file manager for the impatient");

    m_layout->addLayout(m_link_layout);

    this->exec();
}
