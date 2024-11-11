#pragma once

#include <QVBoxLayout>
#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include "Task.hpp"
#include <QHash>

class TasksWidget : public QWidget {
Q_OBJECT
public:
    TasksWidget(QWidget *parent = nullptr);
    void addTask(const Task *task) noexcept;

signals:
    void taskFinished(const Task &task);

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QHash<QUuid, const Task*> m_task_hash;
};