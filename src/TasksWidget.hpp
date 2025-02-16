#pragma once

#include "pch/pch_taskswidget.hpp"

#include "Task.hpp"
#include "TaskManager.hpp"
#include "TaskCardTemplate.hpp"
#include <kddockwidgets-qt6/kddockwidgets/DockWidget.h>

class TasksWidget : public  KDDockWidgets::QtWidgets::DockWidget {
    Q_OBJECT
    public:
    TasksWidget(TaskManager *taskManager);
    void addTask(const Task *task) noexcept;

    void addTaskCard(Task *task) noexcept;
    void removeTaskCard(Task *task) noexcept;

    signals:
    void taskFinished(const Task &task);

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
