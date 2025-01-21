#pragma once

#include "pch_filepathwidget.hpp"

class CustomCompleter : public QCompleter {
public:
  explicit CustomCompleter(QAbstractItemModel *model = nullptr,
                           QWidget *parent = nullptr)
  : QCompleter(model, parent) {}

protected:
    QStringList splitPath(const QString &path) const override {
        QString modifiedPath = path;

        if (path.startsWith("~")) {
            modifiedPath.replace(0, 1, QDir::homePath());
        }

        return QCompleter::splitPath(modifiedPath);
    }

    QString pathFromIndex(const QModelIndex& index) const override
    {
        if (!index.isValid()) { return QString(); }

        QModelIndex idx = index;
        QStringList list;
        do {
            QString t = model()->data(idx, Qt::EditRole).toString();
            list.prepend(t);
            QModelIndex parent = idx.parent();
            idx = parent.sibling(parent.row(), index.column());
        } while (idx.isValid());

        list[0].clear() ; // the join below will provide the separator

        return list.join("/");
    }

};

class FilePathLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit FilePathLineEdit(QWidget *parent = nullptr)
    : QLineEdit(parent) {
        this->setReadOnly(true);
        this->setFrame(false);
    }

    void Focus() noexcept {
        this->setReadOnly(false);
        this->selectAll();
        this->setFocus();
        m_original_path = this->text();
}

signals:
    void tabPressed();

protected:
    void mouseDoubleClickEvent(QMouseEvent *e) override { Focus(); }

    void keyPressEvent(QKeyEvent *e) override {
        if (e->key() == Qt::Key_Escape) {
            this->setReadOnly(true);
            if (!(m_original_path.isEmpty() && m_original_path.isNull()))
                this->setText(m_original_path);
            this->clearFocus();
        }

        else if (e->key() == Qt::Key_Tab) {
            emit tabPressed();
            e->accept();
        }

        else
            QLineEdit::keyPressEvent(e);
    }

private:
    QString m_original_path;
};

class FilePathWidget : public QWidget {
    Q_OBJECT
public:
    FilePathWidget(QWidget *parent = nullptr);
    void setCurrentDir(const QString& path) noexcept;
    void FocusLineEdit() noexcept { m_path_line->Focus(); }
    void setForegroundColor(const QString &color) noexcept;
    void setBackgroundColor(const QString &color) noexcept;
    void setItalic(const bool &state) noexcept;
    void setBold(const bool &state) noexcept;
    void set_font_family(const QString &name) noexcept;

    inline bool bold() noexcept { return m_bold; }
    inline bool italic() noexcept { return m_italic; }

    inline int get_font_size() noexcept {
        return font().pixelSize();
    }

    inline void set_font_size(const int &size) noexcept {
        QFont _font = font();
        _font.setPixelSize(size);
        setFont(_font);
    }

signals:
    void directoryChangeRequested(const QString& path);

private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    FilePathLineEdit *m_path_line = new FilePathLineEdit();
    CustomCompleter *m_completer = nullptr;
    QFileSystemModel *m_completer_model = new QFileSystemModel(this);

    bool m_italic = false, m_bold = false;
};
