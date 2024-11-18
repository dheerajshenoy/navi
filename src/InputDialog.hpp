#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QEventLoop>

class InputDialog : public QDialog {

public:
    InputDialog(QWidget *parent = nullptr);

    QString getText(const QString &title, const QString &text, bool &ok) noexcept;

private:
    QVBoxLayout *m_layout = new QVBoxLayout(this);
    QLineEdit *m_line_edit = new QLineEdit(this);
    QPushButton *m_ok_btn = new QPushButton("Ok", this);
    QLabel *m_msg_label = new QLabel(this);
    QPushButton *m_cancel_btn = new QPushButton("Cancel", this);
    QHBoxLayout *m_btn_layout = new QHBoxLayout(this);
    QEventLoop eventLoop;
};