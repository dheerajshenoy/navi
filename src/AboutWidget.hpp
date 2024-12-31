#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

class AboutWidget : public QDialog {

public:
    explicit AboutWidget(QWidget *parent = nullptr);

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QPushButton *m_github_btn = new QPushButton("Github");
    QPushButton *m_linkedin_btn = new QPushButton("LinkedIn");
    QLabel *m_sub_label = new QLabel();
    QLabel *m_main_label = new QLabel("Navi");

};
