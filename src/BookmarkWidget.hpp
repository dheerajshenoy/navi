#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QHeaderView>

#include "BookmarkManager.hpp"

class BookmarkWidget : public QDialog {
    Q_OBJECT
public:
    explicit BookmarkWidget(BookmarkManager *manager = nullptr, QWidget *parent = nullptr);

    inline void setManager(BookmarkManager *manager) noexcept {
        m_manager = manager;
    }

    void show() noexcept {
        emit visibilityChanged(true);
        QDialog::show();
    }

    void hide() noexcept {
        emit visibilityChanged(false);
        QDialog::hide();
    }

    void loadBookmarks() noexcept;

signals:
    void visibilityChanged(const bool &state);
    void bookmarkGoRequested(const QString &name);

private:

    QVBoxLayout *m_layout = nullptr;
    QTableWidget *m_table_widget = nullptr;
    BookmarkManager *m_manager = nullptr;
};