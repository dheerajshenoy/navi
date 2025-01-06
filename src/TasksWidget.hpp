#pragma once

#include "pch_taskswidget.hpp"

#include "Task.hpp"
#include "TaskManager.hpp"
#include "TaskCardTemplate.hpp"

class TasksWidget : public QWidget {
Q_OBJECT
public:
  TasksWidget(TaskManager *taskManager, QWidget *parent = nullptr);
    void addTask(const Task *task) noexcept;

    inline void show() noexcept {
        emit visibilityChanged(true);
        QWidget::show();
    }

    inline void hide() noexcept {
        emit visibilityChanged(false);
        QWidget::show();
    }

    void addTaskCard(Task *task) noexcept;
    void removeTaskCard(Task *task) noexcept;

signals:
    void taskFinished(const Task &task);
    void visibilityChanged(const bool &state);

private:
    void updateNoTasksLabel() noexcept;
    QVBoxLayout *m_layout = new QVBoxLayout(this);
    QVBoxLayout *taskLayout;
    TaskManager *m_task_manager;
    QHash<const QUuid, TaskCardTemplate*> m_task_cards;
    QScrollArea *scrollArea;
    QLabel *m_no_tasks_label = new QLabel("No tasks running", this);
    QWidget *taskContainer;
};
