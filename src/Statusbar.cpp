#include "Statusbar.hpp"

Statusbar::Statusbar(QWidget *parent) : QWidget(parent) {
  this->setLayout(m_vert_layout);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

  m_layout->setContentsMargins(10, 0, 10, 10);
  m_vert_layout->setContentsMargins(10, 0, 10, 0);

  m_message_label->hide();
  m_vert_layout->addLayout(m_layout);
  m_layout->addWidget(m_visual_line_mode_label);
  m_layout->addSpacing(10);
  m_layout->addWidget(m_file_name_label);
  m_layout->addStretch();
  m_layout->addWidget(m_filter_label);
  m_layout->addSpacing(10);
  m_layout->addWidget(new QLabel("Items: "));
  m_layout->addWidget(m_num_items_label);
  m_layout->addSpacing(10);
  m_layout->addWidget(m_file_size_label);
  m_layout->addSpacing(10);
  m_layout->addWidget(m_file_modified_label);
  m_layout->addSpacing(10);
  m_layout->addWidget(m_file_perm_label);

  m_visual_line_mode_label->setHidden(true);

  m_vert_layout->addWidget(m_message_label);
  m_filter_label->setHidden(true);
  m_message_palette = m_message_label->palette();
}

void Statusbar::Message(const QString &message, const MessageType type,
                        int ms) noexcept {

  switch (type) {
  case MessageType::INFO:
    m_message_palette.setColor(m_message_label->foregroundRole(), Qt::white);
    break;

  case MessageType::WARNING:
    m_message_palette.setColor(m_message_label->foregroundRole(), Qt::yellow);
    break;

  case MessageType::ERROR:
    m_message_palette.setColor(m_message_label->foregroundRole(), Qt::red);
    break;
  }

  m_message_label->setPalette(m_message_palette);
  m_message_label->setText(message);

  m_message_label->show();
  QTimer::singleShot(ms * 1000, [&]() { m_message_label->hide(); });

  emit logMessage(message, type);
}

// Set the number of items in the directory
void Statusbar::SetNumItems(const int &numItems) noexcept {
  m_num_items_label->setText(QString::number(numItems));
}

void Statusbar::SetFile(const QString &file) noexcept {
  m_file_path = file;
  QFileInfo fileInfo(file);
  m_file_name_label->setText(fileInfo.fileName());
  m_file_perm_label->setText(utils::getPermString(fileInfo));
  m_file_size_label->setText(m_locale.formattedDataSize(fileInfo.size()));
  m_file_modified_label->setText(fileInfo.lastModified().toString());
}

void Statusbar::UpdateFile() noexcept {
  QFileInfo fileInfo(m_file_path);
  m_file_name_label->setText(fileInfo.fileName());
  m_file_perm_label->setText(utils::getPermString(fileInfo));
  m_file_size_label->setText(m_locale.formattedDataSize(fileInfo.size()));
  m_file_modified_label->setText(fileInfo.lastModified().toString());
}

void Statusbar::SetFilterMode(const bool state) noexcept {
  if (state)
    m_filter_label->show();
  else
    m_filter_label->hide();
}

Statusbar::~Statusbar() {}

void Statusbar::SetVisualLineMode(const bool &state) noexcept {
  if (state)
    m_visual_line_mode_label->show();
  else
    m_visual_line_mode_label->hide();
}