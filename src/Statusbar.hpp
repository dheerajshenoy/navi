#pragma once

#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPalette>
#include <QSizePolicy>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include "utils.hpp"

enum class MessageType { INFO = 0, WARNING, ERROR };

class Statusbar : public QWidget {
    Q_OBJECT


signals:
  void visibilityChanged(const bool &state);
    void logMessage(const QString &message, const MessageType &type);

public:
    Statusbar(QWidget *parent = nullptr);
    ~Statusbar();

    void SetVisualLineMode(const bool &state) noexcept;
    void Message(const QString &message, MessageType type = MessageType::INFO,
                 int ms = 2) noexcept;
    void SetFile(const QString &filename) noexcept;
    void UpdateFile() noexcept;
    void SetFilterMode(const bool state) noexcept;
    void SetNumItems(const int &numItems) noexcept;

    void show() noexcept {
        emit visibilityChanged(true);
        QWidget::show();
    }

    void hide() noexcept {
        emit visibilityChanged(false);
        QWidget::hide();
    }

private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    QVBoxLayout *m_vert_layout = new QVBoxLayout();
    QLabel *m_message_label = new QLabel();
    QLabel *m_file_name_label = new QLabel();
    QLabel *m_file_size_label = new QLabel();
    QLabel *m_file_perm_label = new QLabel();
    QLabel *m_file_modified_label = new QLabel();
    QLabel *m_filter_label = new QLabel("FILTER");
    QLabel *m_num_items_label = new QLabel();
    QLabel *m_visual_line_mode_label = new QLabel("VISUAL LINE");

    QLocale m_locale;
    QString m_file_path;
    QPalette m_message_palette;
};