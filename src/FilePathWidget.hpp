#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QCompleter>
#include <QFileSystemModel>

class CustomCompleter : public QCompleter {
public:
  explicit CustomCompleter(QAbstractItemModel *model = nullptr,
                           QWidget *parent = nullptr)
  : QCompleter(model, parent) {}

protected:
    QStringList splitPath(const QString &path) const override {
        QString modifiedPath = path;

        if (path.startsWith("~"))
            modifiedPath.replace(0, 1, QDir::homePath());

        return QCompleter::splitPath(modifiedPath);
    }
};

class FilePathLineEdit : public QLineEdit {

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

protected:
    void mouseDoubleClickEvent(QMouseEvent *e) override { Focus(); }

    void keyPressEvent(QKeyEvent *e) override {
        if (e->key() == Qt::Key_Escape) {
            this->setReadOnly(true);
            if (!(m_original_path.isEmpty() && m_original_path.isNull()))
                this->setText(m_original_path);
            this->clearFocus();
        } else
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
    void setFont(const QString &name) noexcept;

signals:
    void directoryChangeRequested(const QString& path);

private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    FilePathLineEdit *m_path_line = new FilePathLineEdit();
    CustomCompleter *m_completer = nullptr;
    QFileSystemModel *m_completer_model = new QFileSystemModel(this);
};