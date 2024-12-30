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

            disconnect(this, &LineEdit::returnPressed, 0, 0);
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
    void setForeground(const std::string &foreground) noexcept;
    void setBackground(const QString &background) noexcept;
    void setBackground(const std::string &background) noexcept;
    void setFontFamily(const QString &family) noexcept;
    void setFontFamily(const std::string &family) noexcept;
    void addCompletionStringList(const CompletionModelType &type,
                            const QStringList &stringList) noexcept;
    void currentCompletionStringList(const CompletionModelType &type) noexcept;

    // returns the QLineEdit
    LineEdit *lineEdit() noexcept { return m_line_edit; }

    // returns the font family
    inline std::string get_font() noexcept { return font().family().toStdString(); }

    // sets the font
    void set_font_size(const int &size = 14) noexcept;

    // returns the font pixel size
    inline int get_font_size() noexcept {
        return font().pixelSize();
    }

    // return the background color
    inline std::string Get_background_color() noexcept { return m_background_color.toStdString(); }

    // return the foreground color
    inline std::string Get_foreground_color() noexcept { return m_foreground_color.toStdString(); }

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
    QString m_background_color,
            m_foreground_color;
};
