#pragma once

#include <QHeaderView>

class TableHeaderView : public QHeaderView {
Q_OBJECT
public:
  explicit TableHeaderView(Qt::Orientation orientation,
                           QWidget *parent = nullptr) : QHeaderView(orientation, parent) {

    setStretchLastSection(true);
}

    void hide() noexcept {
        emit visibilityChanged(false);
        QHeaderView::hide();
    }

    void show() noexcept {
        emit visibilityChanged(true);
        QHeaderView::show();
    }


    signals:
    void visibilityChanged(const bool &state);
};