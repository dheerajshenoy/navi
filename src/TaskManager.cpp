#include "TaskManager.hpp"

TaskManager::TaskManager(QObject *parent) :QObject(parent) {}

void TaskManager::addTask(Task *task) noexcept {
    m_tasks_hash.insert(task->uuid(), task);
    task->run();

    connect(task, &Task::finished, this, [&](const QUuid &uuid) {
      // removeTask(uuid); TODO: remove only if auto remove option is
      // enabled
      emit taskFinished(uuid);
    });

    emit taskAdded(task);
}

bool TaskManager::removeTask(const QUuid &uuid) noexcept {
    if (!m_tasks_hash.contains(uuid))
        return false;
    delete m_tasks_hash.value(uuid);
    if (m_tasks_hash.remove(uuid)) {
        return true;
    }
    return false;
}

bool TaskManager::clearTasks() noexcept {
    if (m_tasks_hash.isEmpty())
        return false;
    m_tasks_hash.clear();
    emit tasksCleared();
    return true;
}

unsigned int TaskManager::taskCount() const noexcept {
    return m_tasks_hash.size();
}

const Task* TaskManager::task(const QUuid &uuid) const noexcept {
    return m_tasks_hash.isEmpty() ? nullptr : m_tasks_hash.value(uuid);
}

QList<Task *> TaskManager::tasks() const noexcept {
    return m_tasks_hash.values();
}

void TaskManager::cancelTask(const QUuid &uuid) noexcept {
    if (m_tasks_hash.contains(uuid)) {
        auto task = m_tasks_hash[uuid];
        task->cancel();
    }
}
