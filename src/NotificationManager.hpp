#pragma once

#include <QObject>
#include <QTimer>
#include <QEvent>
#include "NotificationDialog.hpp"

class NotificationManager : public QObject {
	Q_OBJECT

public:

    explicit NotificationManager(QObject *parent = nullptr);

    void showMessage(QWidget *widget, NotificationDialog::NotificationType type,
                     const QString &message, const bool &icon = true) noexcept;

    void positionNotifications() noexcept;
    void repositionNotification(NotificationDialog *notification, QWidget *widget) noexcept;
    void removeNotification(NotificationDialog *notification) noexcept;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QSize parentSize = QSize(0, 0);
    QTimer *timer;
    QList<NotificationDialog *> activeNotifications;
};
