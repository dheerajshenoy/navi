#pragma once

#include <QWidget>
#include <QListView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QStringListModel>
#include "LineEdit.hpp"
#include <QSortFilterProxyModel>
#include "CompletionDelegate.hpp"

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
        setFixedHeight(height);
    }
    inline int popupHeight() noexcept { return this->height(); }

    inline void set_font_family(const std::string &name) noexcept {
        QFont font = this->font();
        font.setFamily(QString::fromStdString(name));
        this->setFont(font);
    }

    inline void set_font_size(const int &size) noexcept {
        QFont font = this->font();
        font.setPixelSize(size);
        this->setFont(font);
    }

    void showPopup() noexcept;
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
        m_completion_delegate->setLineNumbers(state);
    }

    inline bool lineNumbers() noexcept { return m_completion_delegate->lineNumberShown(); }

    void updatePopupGeometry() noexcept;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void selectItem(const QModelIndex &index) noexcept;
    LineEdit* m_lineEdit;
    QListView *m_listView;
    bool m_show_item_numbers = false;
    QStringListModel *m_model = nullptr;
    CompletionFilterModel *m_filter_model;
    CompletionDelegate *m_completion_delegate;
};
