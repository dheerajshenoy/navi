#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <qboxlayout.h>

class RegisterWidget : public QWidget {

public:
    RegisterWidget(QWidget *parent = nullptr);

private:
    QVBoxLayout *m_layout = new QVBoxLayout(this);
};
