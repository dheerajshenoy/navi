#include "AboutWidget.hpp"

AboutWidget::AboutWidget(QWidget *parent) : QDialog(parent) {

    this->setLayout(m_layout);

    m_layout->addWidget(m_main_label);

    this->exec();
}