#include "RegisterWidget.hpp"
#include <qnamespace.h>

RegisterWidget::RegisterWidget(QWidget *parent) : QWidget(parent) {

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    this->setLayout(m_layout);

}
