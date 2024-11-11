#pragma once

#include <QWidget>
#include <QTreeView>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include "FilePanelWidget.hpp"
#include <QUuid>
#include <QHash>

class MarksListModel : public QStandardItemModel {
    Q_OBJECT
public:
    // Constructor that accepts a pointer to an external QSet
  explicit MarksListModel(QObject *parent = nullptr)
      : QStandardItemModel(parent) {}

  void addSetPTR(QSet<QString> *PTR, const QUuid &uuid) noexcept {
      set_hash.insert(uuid, PTR);
  }

  QHash<QUuid, QSet<QString> *> set_hash; // Pointer to the external QSet

private:
};


class MarksBuffer : public QWidget {
    Q_OBJECT

signals:
void visibilityChanged(const bool& state);

public:
    explicit MarksBuffer(QWidget *parent = nullptr) : QWidget(parent) {
        this->setLayout(m_layout);
        m_layout->addWidget(m_marks_list_view);
        m_marks_list_view->setModel(m_model);

        this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

        QPushButton *m_close_btn = new QPushButton("Close");
        m_layout->addWidget(m_close_btn);

        connect(m_close_btn, &QPushButton::clicked, this, [&]() { this->close(); });
    }

    void show() noexcept {
        emit visibilityChanged(true);
        QWidget::show();
    }

    void hide() noexcept {
        emit visibilityChanged(false);
        QWidget::hide();
    }

    void addMarksSet(QSet<QString> *setPTR, const QUuid &uuid) noexcept {
        m_model->addSetPTR(setPTR, uuid);
    }

    void refreshMarksList() noexcept {
    m_model->clear();
        // Populate the model with the data from the manager
        for (auto it = m_model->set_hash.constBegin(); it != m_model->set_hash.constEnd(); ++it) {
            QStandardItem *tabItem = new QStandardItem(it.key().toString());  // Tab ID as root item

            // Add each file in the QSet as a child item
            for (const QString &fileName : *it.value()) {
                QStandardItem *fileItem = new QStandardItem(fileName);
                tabItem->appendRow(fileItem);
            }

            m_model->appendRow(tabItem);  // Add the tab item as a top-level item
        }
    }

private:
    QTreeView *m_marks_list_view = new QTreeView();
    QVBoxLayout *m_layout = new QVBoxLayout();

    MarksListModel *m_model = new MarksListModel();

};