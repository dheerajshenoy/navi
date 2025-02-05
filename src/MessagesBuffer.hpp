#pragma once

#include "../KDDockWidgets/src/qtwidgets/views/DockWidget.h"
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class MessagesBuffer : public KDDockWidgets::QtWidgets::DockWidget {
  Q_OBJECT

signals:
  void visibilityChanged(const bool &state);

public:
  explicit MessagesBuffer()
      : KDDockWidgets::QtWidgets::DockWidget("Messages Pane") {
    m_layout->addWidget(m_text_edit);
    this->setLayout(m_layout);
    QPushButton *m_clear_btn = new QPushButton("Clear");
    connect(m_clear_btn, &QPushButton::clicked, this,
            [&]() { m_text_edit->clear(); });
    m_layout->addWidget(m_clear_btn);

    m_text_edit->setAcceptRichText(true);
    m_text_edit->setReadOnly(true);
    m_widget->setLayout(m_layout);
    this->setWidget(m_widget);
    this->setFloating(true);
  }

  void AppendText(const QString &text) noexcept { m_text_edit->append(text); }

private:
  QWidget *m_widget = new QWidget();
  QTextEdit *m_text_edit = new QTextEdit();
  QVBoxLayout *m_layout = new QVBoxLayout();
};
