#include "TasksWidget.hpp"

TasksWidget::TasksWidget(QWidget *parent) : QWidget(parent) {

    m_layout->addWidget(new QLabel("Tasks"));
    m_layout->addStretch(1);
    this->setLayout(m_layout);

}

void TasksWidget::addTask(const Task *task) noexcept {
    connect(task, &Task::finished, this, [&](const QUuid &uuid) {
        m_task_hash.remove(uuid);
    });
    m_task_hash.insert(task->uuid(), task);
}