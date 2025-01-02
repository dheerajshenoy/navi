#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QStringList>
#include <QKeyEvent>
#include <QApplication>

class CustomCompleter : public QWidget {
    Q_OBJECT

    public:
    explicit CustomCompleter(QWidget *parent = nullptr);
    void setWidget(QLineEdit *widget); // Attach to a QLineEdit widget
    void setSuggestions(const QStringList &suggestions); // Set suggestions list
    void popup(); // Show popup
    void popup_hide() noexcept;
    void filterSuggestions(const QString &text);
    inline QString current_completion_string() { return m_current_completion_string; }
    inline void set_current_completion_string(const QString &str) {
        m_current_completion_string = str;
    }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void onItemClicked(QListWidgetItem *item);

private:
    QListWidget *listWidget;
    QLineEdit *attachedWidget;
    QStringList suggestionList;
    QString m_current_completion_string;

    void setupConnections();
};

