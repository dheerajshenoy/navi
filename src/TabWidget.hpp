#pragma once

#include <QTabWidget>

class TabWidget : public QTabWidget {
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = nullptr) : QTabWidget(parent) {}

    signals:
    void tabVisibilityChanged(const bool &state);

private:
};