#pragma once

#include <QWidget>
#include <QListView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QStringListModel>
#include "LineEdit.hpp"
#include <QSortFilterProxyModel>

class CompletionFilterModel : public QSortFilterProxyModel {
public:
    CompletionFilterModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        if (filterRegularExpression().pattern().isEmpty()) {
            return true; // Show all items when filter is empty
        }
        return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    }
};

class CompletionPopup : public QFrame {
    Q_OBJECT

    public:
    explicit CompletionPopup(LineEdit *parent);

    inline void setPopupHeight(const int &height) noexcept {
        m_popupHeight = height;
    }

    inline int popupHeight() noexcept { return m_popupHeight; }

    inline bool itemNumber() noexcept { return m_show_item_numbers; }

    inline void setItemNumbersVisible(const bool &state) noexcept {
        m_show_item_numbers = state;
    }

    void showPopup() noexcept;

    void updateAndShowPopup() noexcept;

    void updateCompletions(const QString &text) noexcept;

    void setCompletions(const QStringList &) noexcept;

    inline QStringList completions() noexcept {
        return m_model->stringList();
    }

    inline void setCaseSensitivity(const Qt::CaseSensitivity &sensitivity) noexcept {
        m_filter_model->setFilterCaseSensitivity(sensitivity);
    }

    inline Qt::CaseSensitivity caseSensitivity() noexcept {
        return m_filter_model->filterCaseSensitivity();
    }

    inline void setLineNumbers(const bool &state) noexcept {
        m_line_numbers_shown = state;
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void selectItem(const QModelIndex &index) noexcept;

    LineEdit* m_lineEdit;
    QListView *m_listView;
    int m_popupHeight = 150;
    bool m_show_item_numbers = false;
    bool m_line_numbers_shown = false;
    QStringListModel *m_model = nullptr;
    CompletionFilterModel *m_filter_model;
};
