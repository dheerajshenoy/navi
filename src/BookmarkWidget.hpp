#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QPushButton>

#include "BookmarkManager.hpp"

class BookmarkWidget : public QWidget {
    Q_OBJECT
public:
    explicit BookmarkWidget(BookmarkManager *manager = nullptr, QWidget *parent = nullptr);

    inline void setManager(BookmarkManager *manager) noexcept {
        m_manager = manager;
    }


    void show() noexcept {
        emit visibilityChanged(true);
        QWidget::show();
    }

    void hide() noexcept {
        emit visibilityChanged(false);
        QWidget::hide();
    }


    signals:
    void visibilityChanged(const bool &state);
private:
    QVBoxLayout *m_layout = nullptr;
    QTableWidget *m_table_widget = nullptr;
    BookmarkManager *m_manager = nullptr;
};