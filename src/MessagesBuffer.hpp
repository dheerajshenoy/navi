#pragma once

#include <QTextEdit>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class MessagesBuffer : public QWidget {
    Q_OBJECT

signals:
void visibilityChanged(const bool& state);

public:
    explicit MessagesBuffer(QWidget *parent = nullptr) : QWidget(parent) {
        m_layout->addWidget(m_text_edit);
        this->setLayout(m_layout);
        QPushButton *m_close_btn = new QPushButton("Close");
        connect(m_close_btn, &QPushButton::clicked, this, [&]() { this->close(); });
        m_layout->addWidget(m_close_btn);
        this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

        m_text_edit->setAcceptRichText(true);
        m_text_edit->setReadOnly(true);

    }

    void AppendText(const QString &text) noexcept {
        m_text_edit->append(text);
    }

    void show() noexcept {
        emit visibilityChanged(true);
        QWidget::show();
    }

    void hide() noexcept {
        emit visibilityChanged(false);
        QWidget::hide();
    }

private:
    QTextEdit *m_text_edit = new QTextEdit();
    QVBoxLayout *m_layout = new QVBoxLayout();
};