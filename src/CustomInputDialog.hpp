#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QEventLoop>

class CustomInputDialog : public QDialog {

public:
    CustomInputDialog(QWidget *parent = nullptr);
    QString getText(const QString &title,
                    const QString &text,
                    bool &ok,
                    const QString &selection_text) noexcept;

    inline void setTextValue(const QString &text) noexcept {
        m_msg_label->setText(text);
    }

    inline void setWindowTitle(const QString &title) noexcept {
        this->setWindowTitle(title);
    }

    inline QLineEdit* lineEdit() noexcept { return m_line_edit; }
    inline QString textValue() noexcept { return m_line_edit->text(); }

    enum class DialogCode {
        Accepted = 0,
        Rejected,
    };

protected:

    void keyPressEvent(QKeyEvent *e) noexcept override {

        if (e->key() == Qt::Key_Escape) {
            m_result = CustomInputDialog::DialogCode::Rejected;
        }
    }

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QLineEdit *m_line_edit = new QLineEdit();
    QPushButton *m_ok_btn = new QPushButton("Ok");
    QLabel *m_msg_label = new QLabel();
    QPushButton *m_cancel_btn = new QPushButton("Cancel");
    QHBoxLayout *m_btn_layout = new QHBoxLayout();
    QEventLoop eventLoop;
    CustomInputDialog::DialogCode m_result;
};
