#pragma once

#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QAbstractListModel>

class MarksListModel : public QAbstractListModel {
    Q_OBJECT
public:
    // Constructor that accepts a pointer to an external QSet
    explicit MarksListModel(const QSet<QString>* set = nullptr, QObject *parent = nullptr)
        : QAbstractListModel(parent), m_set(set) {}

    // Override rowCount
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_set ? m_set->size() : 0;
    }

    void setSetPtr(const QSet<QString> *set_ptr) noexcept {
        m_set = set_ptr;
    }

    // Override data
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || role != Qt::DisplayRole || !m_set)
            return QVariant();
        return *std::next(m_set->begin(), index.row());
    }

    // Public method to notify the model of external updates
    void refreshModel() noexcept {
        beginResetModel();
        endResetModel();
    }

private:
    const QSet<QString>* m_set;  // Pointer to the external QSet
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

    void setMarksSet(const QSet<QString> *setPTR) noexcept {
        m_model->setSetPtr(setPTR);
    }

    void refreshMarksList() noexcept {
        m_model->refreshModel();
    }

private:
    QListView *m_marks_list_view = new QListView();
    QVBoxLayout *m_layout = new QVBoxLayout();


    MarksListModel *m_model = new MarksListModel();

};