#pragma once

#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>
#include "Statusbar.hpp"

class NotificationDialog : public QDialog {
  Q_OBJECT

public:
  explicit NotificationDialog(QWidget *parent = nullptr);
  ~NotificationDialog();

  enum class NotificationType {
    ERROR = 0,
    WARNING,
    INFO,
  };

  void showNotification(const QPoint &position, const QString &message,
                        NotificationType type, const bool &icon = true) noexcept;

  void animateDialogPosition(QDialog *dialog, const QPoint &start,
                             const QPoint &end) noexcept;

signals:
  void closeRequested(NotificationDialog *);

protected:
  void showEvent(QShowEvent *event) override {
    QDialog::showEvent(event);
    animateShow();
  }

  void closeEvent(QCloseEvent *event) override {
    animateHide();
    event->ignore(); // Ignore default close so animation can complete
  }

private:
  void animateShow() noexcept;
  void animateHide() noexcept;

  QHBoxLayout *m_icon_message_layout = new QHBoxLayout();
  QLabel *m_icon_label = new QLabel();
  QLabel *m_message_label = new QLabel();
  QVBoxLayout *m_layout = new QVBoxLayout();
  QPushButton *m_closebtn = new QPushButton("Close");
  QHBoxLayout *m_hlayout = new QHBoxLayout();
  QGraphicsOpacityEffect *m_opacity_effect;
};
