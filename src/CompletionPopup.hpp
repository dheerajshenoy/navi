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

    inline std::string get_font_family() noexcept {
        return this->font().family().toStdString();
    }

    inline void setGrid(const bool &state) noexcept {
        m_completion_delegate->setGrid(state);
    }

    inline bool grid() noexcept { return m_completion_delegate->grid(); }

    inline void set_font_size(const int &size) noexcept {
        QFont font = this->font();
        font.setPixelSize(size);
        this->setFont(font);
    }

    inline int get_font_size() noexcept {
        return this->font().pixelSize();
    }

    void showPopup() noexcept;
    void updateCompletions(const QString &text) noexcept;
    void setCompletions(const QStringList &completions) noexcept;
    void setCompletions(const QString &mode,
                        const QStringList &completions) noexcept;

    inline QStringList completions() noexcept {
        return m_completion_hash[m_current_mode];
        // return m_model->stringList();
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

    inline void setMatchCount(const bool &state) noexcept {
        m_match_count_visible = state;
    }

    inline bool matchCount() noexcept { return m_match_count_visible; }

    inline QString mode() noexcept { return m_current_mode; }

    void setMode(const QString &mode) noexcept;

    void updatePopupGeometry() noexcept;

    protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

    signals:
    void matchCountSignal(int count, int total);

    private:
    void selectItem(const QModelIndex &index) noexcept;
    LineEdit* m_lineEdit;
    QListView *m_listView;
    bool m_show_item_numbers = false;
    QStringListModel *m_model = nullptr;
    CompletionFilterModel *m_filter_model;
    CompletionDelegate *m_completion_delegate;
    bool m_match_count_visible = false;
    int m_total_completions_count = -1;
    QString m_current_mode = "none";
    QHash<QString, QStringList> m_completion_hash;
};
