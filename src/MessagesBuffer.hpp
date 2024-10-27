#pragma once

#include <QTextEdit>

class MessagesBuffer : public QTextEdit {
    Q_OBJECT

signals:
void visibilityChanged(const bool& state);

public:
    explicit MessagesBuffer(QWidget *parent = nullptr) : QTextEdit(parent) {}

    void show() noexcept {
        emit visibilityChanged(true);
        QTextEdit::show();
    }

    void hide() noexcept {
        emit visibilityChanged(false);
        QTextEdit::hide();

    }
};