#include "NotificationManager.hpp"

NotificationManager::NotificationManager(QObject *parent) : QObject{parent} {
	timer = new QTimer(this);
}

bool NotificationManager::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::Resize) {
        QWidget* resizedWidget = qobject_cast<QWidget*>(watched);

        if (resizedWidget) {
            for (auto notification : activeNotifications) {
                if (notification->property("sourceWidget").value<QWidget*>() == resizedWidget) {
                    repositionNotification(notification, resizedWidget);
                }
            }
        }
    }

    return false;
}

void NotificationManager::showMessage(QWidget *widget,
									  NotificationDialog::NotificationType type,
									  const QString &message,
									  const bool &icon) noexcept
{
    int duration = 0;

    switch (static_cast<int>(type)) {
	case static_cast<int>(NotificationDialog::NotificationType::WARNING):
        duration = 5000;
        break;

    case static_cast<int>(NotificationDialog::NotificationType::INFO):
        duration = 3000;
        break;

    case static_cast<int>(NotificationDialog::NotificationType::ERROR):
        duration = 7000;
        break;

    default:
        break;
    }

    parentSize = widget->size();
    widget->installEventFilter(this);

    NotificationDialog *nd = new NotificationDialog(widget);

    int x = parentSize.width() - (nd->width() + 20);
    int y = parentSize.height() - ((activeNotifications.count() + 1)  * (nd->height() + 20));

    QPoint position(x, y);

    nd->setProperty("sourceWidget", QVariant::fromValue(widget));
    nd->showNotification(position, message, type, icon);

    activeNotifications.append(nd);

    QTimer::singleShot(duration, this,
                       [this, nd]() { removeNotification(nd); });

    connect(nd, &NotificationDialog::closeRequested, this,
            &NotificationManager::removeNotification);
}

void NotificationManager::positionNotifications() noexcept {
    for (int i = 0; i < activeNotifications.count(); i++) {
		auto notification = activeNotifications.at(i);

        int x = parentSize.width() - (notification->width() + 20);
        int y = parentSize.height() - ((i + 1) * (notification->height() + 20));
        QPoint position(x, y);

        notification->move(position);
    }
}

void NotificationManager::repositionNotification(NotificationDialog *notification, QWidget *widget) noexcept {
    parentSize = widget->size();
    positionNotifications();
}

void NotificationManager::removeNotification(NotificationDialog *notification) noexcept {
    activeNotifications.removeOne(notification);
    notification->close();
    positionNotifications();
}
