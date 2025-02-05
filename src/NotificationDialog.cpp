#include "NotificationDialog.hpp"

NotificationDialog::NotificationDialog(QWidget *parent) : QDialog(parent) {
    setWindowFlags(Qt::FramelessWindowHint);

	m_opacity_effect = new QGraphicsOpacityEffect(this);
	setGraphicsEffect(m_opacity_effect);

	setModal(false);
    setSizeGripEnabled(false);

    // INIT LAYOUT

    this->setWindowTitle("Notification from Navi");

    m_message_label->setTextFormat(Qt::RichText);
    m_hlayout->addStretch(1);
    m_hlayout->addWidget(m_closebtn);

    this->setGeometry(0, 0, 500, 100);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setMinimumSize(QSize(500, 100));
    this->setMaximumSize(QSize(500, 100));

    m_icon_label->hide();

    m_icon_message_layout->addWidget(m_icon_label);
    m_icon_message_layout->addWidget(m_message_label);
    m_icon_message_layout->addStretch(1);

    m_layout->addLayout(m_icon_message_layout);
    m_layout->addLayout(m_hlayout);

	connect(m_closebtn, &QPushButton::clicked, this, [&]() {
		emit closeRequested(this);
	});

    this->setLayout(m_layout);
}

NotificationDialog::~NotificationDialog()
{}

void NotificationDialog::showNotification(const QPoint &position,
										  const QString &message,
										  NotificationType type,
										  const bool &icon) noexcept {
	QString style;
	switch (type) {
	case NotificationType::INFO:
          style = "QDialog {"
                  "   background-color: rgba(0, 85, 127, .8);"
                  "   border-radius: 2px;" // Rounded corners
                  "   padding: 10px;"
                  "}";
        if (icon) {
			QIcon infoIcon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation);
			m_icon_label->setPixmap(infoIcon.pixmap(QSize(32, 32)));
			m_icon_label->show();
        }
        
        break;

    case NotificationType::ERROR:
      style = "QDialog {"
              "   background-color: rgba(153, 0, 0, .8);"
              "   border-radius: 2px;" // Rounded corners
              "   padding: 10px;"
              "}";

        if (icon) {
			QIcon errorIcon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical);
			m_icon_label->setPixmap(errorIcon.pixmap(QSize(32, 32)));

			m_icon_label->show();
        }
        break;

    case NotificationType::WARNING:
      style = "QDialog {"
              "   background-color: rgba(185, 113, 0, .8);"
              "   border-radius: 2px;" // Rounded corners
              "   padding: 10px;"
              "}";

        if (icon) {
			QIcon warningIcon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning);
			m_icon_label->setPixmap(warningIcon.pixmap(QSize(32, 32)));

			m_icon_label->show();
        }
        break;

	default:
		break;
	}

	setStyleSheet(style);
    this->move(position);
    m_message_label->setText(message);
	this->show();
}

void NotificationDialog::animateShow() noexcept {
	QPropertyAnimation *anim = new QPropertyAnimation(m_opacity_effect, "opacity");
	anim->setDuration(400);
	anim->setStartValue(0.0);
	anim->setEndValue(1.0);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void NotificationDialog::animateHide() noexcept {
    QPropertyAnimation *anim = new QPropertyAnimation(m_opacity_effect, "opacity");
    anim->setDuration(400);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);

    connect(anim, &QPropertyAnimation::finished, this, &QDialog::accept);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
