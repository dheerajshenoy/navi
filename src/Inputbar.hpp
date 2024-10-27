#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QEventLoop>
#include <QAbstractItemModel>
#include <QCompleter>
#include <QKeyEvent>

class LineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

    signals:
    void hideRequested();

protected:
    void keyPressEvent(QKeyEvent *e) override {

        if (e->key() == Qt::Key_Escape) {
            this->clearFocus();
            emit hideRequested();
        } else
            QLineEdit::keyPressEvent(e);
    }
};

class Inputbar : public QWidget {
public:
    Inputbar(QWidget *parent = nullptr);
    ~Inputbar();

    QString getInput(const QString& prompt) noexcept;
    void setCompleterModel(QAbstractItemModel *model) noexcept;

private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    QLabel *m_prompt_label = new QLabel();
    LineEdit *m_line_edit = new LineEdit();
    QCompleter *m_line_edit_completer = new QCompleter();
};