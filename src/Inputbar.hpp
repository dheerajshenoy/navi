#pragma once

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QCompleter>
#include <QEventLoop>
#include <QFont>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QWidget>
#include <qnamespace.h>
#include <qsortfilterproxymodel.h>
#include "FilePathWidget.hpp"
#include "OrderlessFilterModel.hpp"
#include <QStringListModel>
#include <qtmetamacros.h>

class InputbarCompleter : public QCompleter {

public:
    InputbarCompleter(QSortFilterProxyModel *model, QWidget *parent = nullptr) : QCompleter(model, parent){}

protected:
    QStringList splitPath(const QString &path) const override {
      QStringList paths = path.split(" ", Qt::SkipEmptyParts);
      return paths;
      // return QCompleter::splitPath(path);
    }

    QString pathFromIndex(const QModelIndex &index) const override {
        auto dd = QCompleter::pathFromIndex(index);
        qDebug() << dd;
        return dd;
    }

};

class LineEdit : public QLineEdit {
    Q_OBJECT

    public:
      explicit LineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {

        this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
      }

    signals:
    void hideRequested();
    void tabPressed();

protected:
    void keyPressEvent(QKeyEvent *e) override {

        if (e->key() == Qt::Key_Escape) {
            this->clearFocus();
            this->clear();
            emit hideRequested();
        }

        else if (e->type() == QKeyEvent::KeyPress) {
            switch (e->key()) {
            case Qt::Key_Tab:
              emit tabPressed();
            e->accept();
                return;
                break;

            }
            QLineEdit::keyPressEvent(e);
        }
        else
            QLineEdit::keyPressEvent(e);
    }
};

class Inputbar : public QWidget {
    Q_OBJECT
public:
    Inputbar(QWidget *parent = nullptr);
    ~Inputbar();

    QString getInput(const QString &prompt, const QString &defaultValue = 0,
                     const QString &selectionString = 0) noexcept;

    enum CompletionModelType {
      COMMAND = 0,
      LUA_FUNCTIONS
    };
    void enableCommandCompletions() noexcept;
    void disableCommandCompletions() noexcept;
    void setFontItalic(const bool &state) noexcept;
    void setFontBold(const bool &state) noexcept;
    void setForeground(const QString &foreground) noexcept;
    void setBackground(const QString &background) noexcept;
    void setFontFamily(const QString &family) noexcept;
    void addCompletionStringList(const CompletionModelType &type,
                            const QStringList &stringList) noexcept;
    void currentCompletionStringList(const CompletionModelType &type) noexcept;

    LineEdit *lineEdit() noexcept { return m_line_edit; }

signals:
    void lineEditTextChanged(const QString &text);

private:
    void suggestionComplete() noexcept;
    QHBoxLayout *m_layout = new QHBoxLayout();
    QLabel *m_prompt_label = new QLabel();
    LineEdit *m_line_edit = new LineEdit();
    InputbarCompleter *m_line_edit_completer = nullptr;
    QStringListModel *m_completer_model = new QStringListModel(this);
    QStringList m_lua_function_stringlist = {};
    OrderlessFilterModel *m_filter_model = nullptr;
    QHash<CompletionModelType, QStringList> m_completion_list_hash;
};