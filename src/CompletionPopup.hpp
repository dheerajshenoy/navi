#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QKeyEvent>
#include "LineEdit.hpp"

class CompletionPopup : public QFrame {
    Q_OBJECT

public:
    explicit CompletionPopup(LineEdit *parent);
    inline void setPopupHeight(const int &height) noexcept {
        m_popupHeight = height;
    }
    inline int popupHeight() noexcept { return m_popupHeight; }
    inline bool itemNumber() noexcept { return m_show_item_numbers; }
    // TODO: Showing item numbers in the QListWidget
    inline void setItemNumbersVisible(const bool &state) noexcept {
        m_show_item_numbers = state;
    }
    void setCompletions(const QStringList& completions) noexcept;
    void showPopup() noexcept;
    void updateCompletions(const QString &text) noexcept;

    inline void setCaseSensitivity(const Qt::CaseSensitivity &sensitivity) noexcept {
        m_case_sensitivity = sensitivity;
    }

    inline Qt::CaseSensitivity caseSensitivity() noexcept {
        return m_case_sensitivity;
    }

    inline void setInitialCompletions(const QStringList &initial_completions) noexcept {
      m_initial_completions = initial_completions;
      this->setCompletions(m_initial_completions);
    }

    inline QStringList initialCompletion() noexcept {
      return m_initial_completions;
    }

    inline QStringList currentCompletion() noexcept {
        return m_current_completions;
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void selectItem(QListWidgetItem* item) noexcept;

    LineEdit* m_lineEdit;
    QListWidget *m_listWidget;
    int m_popupHeight = 150;
    bool m_show_item_numbers = false;
    Qt::CaseSensitivity m_case_sensitivity = Qt::CaseSensitivity::CaseSensitive;
    QStringList m_current_completions;
    QStringList m_initial_completions;
};
