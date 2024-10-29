#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QEventLoop>
#include <QAbstractItemModel>
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>

class LineEdit : public QLineEdit {
    Q_OBJECT

public:
  explicit LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {
  }

    signals:
    void hideRequested();
    void tabPressed();

protected:
    void keyPressEvent(QKeyEvent *e) override {

        if (e->key() == Qt::Key_Escape) {
            this->clearFocus();
            emit hideRequested();
        }

        else if (e->type() == QKeyEvent::KeyPress && e->key() == Qt::Key_Tab) {
            emit tabPressed();
            return;
}

        else
            QLineEdit::keyPressEvent(e);
    }
};

class Inputbar : public QWidget {
public:
    Inputbar(QWidget *parent = nullptr);
    ~Inputbar();

    QString getInput(const QString &prompt, const QString &defaultValue = 0,
                     const QString &selectionString = 0) noexcept;

    void setCompleterModel(QAbstractItemModel *model) noexcept;
    void enableCommandCompletions() noexcept;
    void disableCommandCompletions() noexcept;

private:
    void suggestionComplete() noexcept;

    QHBoxLayout *m_layout = new QHBoxLayout();
    QLabel *m_prompt_label = new QLabel();
    LineEdit *m_line_edit = new LineEdit();
    QCompleter *m_line_edit_completer = new QCompleter();
};