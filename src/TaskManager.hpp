#pragma once

#include <QObject>
#include <QHash>
#include <QUuid>

#include "Task.hpp"

class TaskManager : public QObject {
Q_OBJECT
public:
    TaskManager(QObject *object = nullptr);

    void addTask(Task *task) noexcept;
    bool removeTask(const QUuid &uuid) noexcept;
    bool clearTasks() noexcept;
    unsigned int taskCount() const noexcept;
    const Task* task(const QUuid &uuid) const noexcept;
    QList<Task *> tasks() const noexcept;

    signals:
    void taskAdded(Task *task);
    void taskRemoved(Task *task);
    void tasksCleared();

private:
    QHash<const QUuid, Task*> m_tasks_hash;
};