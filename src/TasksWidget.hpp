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
    void removeTaskCard(const QUuid &uuid) noexcept;

private:
    void updateNoTasksLabel() noexcept;
    QVBoxLayout *m_layout = new QVBoxLayout();
    QVBoxLayout *taskLayout;
    TaskManager *m_task_manager;
    QHash<const QUuid, TaskCardTemplate*> m_task_cards;
    QScrollArea *scrollArea;
    QLabel *m_no_tasks_label = new QLabel("No tasks running", this);
    QWidget *taskContainer;
};
