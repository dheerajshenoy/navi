#pragma once

#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>

class LineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) { setFocusPolicy(Qt::FocusPolicy::ClickFocus); }

signals:
    void hideRequested();
    void tabPressed();
    void spacePressed();

protected:
    bool event(QEvent *event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *e = static_cast<QKeyEvent *>(event);

            if (e->key() == Qt::Key_Tab) {
                emit tabPressed();
                return true;
            }

            else if (e->key() == Qt::Key_Space) {
                emit spacePressed();
            }
        }
        return QLineEdit::event(event);
    }
};
