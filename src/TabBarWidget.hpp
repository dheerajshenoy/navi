#pragma once

#include <QTabWidget>

class TabBarWidget : public QTabWidget {
    Q_OBJECT
  public:

  void show() noexcept { emit visibilityChanged(true); }
void hide() noexcept { emit visibilityChanged(false); }

signals:
    void visibilityChanged(const bool &state);
};