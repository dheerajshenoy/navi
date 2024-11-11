#include "TasksWidget.hpp"
#include <qnamespace.h>

TasksWidget::TasksWidget(TaskManager *taskManager, QWidget *parent)
: QWidget(parent), m_task_manager(taskManager) {

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    taskContainer = new QWidget(this);
    taskLayout = new QVBoxLayout(taskContainer);
    taskContainer->setLayout(taskLayout);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    taskContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    // Add a spacer to the end of the task layout
    auto taskSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    taskLayout->addItem(taskSpacer);
    scrollArea->setWidget(taskContainer);
    m_layout->addWidget(scrollArea);
    this->setLayout(m_layout);
    connect(m_task_manager, &TaskManager::taskAdded, this, &TasksWidget::addTaskCard);
    connect(m_task_manager, &TaskManager::taskRemoved, this,
            &TasksWidget::removeTaskCard);
    m_no_tasks_label->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_no_tasks_label);

    updateNoTasksLabel();
}

void TasksWidget::updateNoTasksLabel() noexcept {
    m_no_tasks_label->setVisible(m_task_cards.isEmpty());
    taskContainer->setVisible(!m_task_cards.isEmpty());
}

void TasksWidget::addTaskCard(Task *task) noexcept {
    auto *taskCard = new TaskCardTemplate(task, this);
    m_task_cards[task->uuid()] = taskCard;
    taskLayout->insertWidget(0, taskCard);
    connect(taskCard, &TaskCardTemplate::taskCancelRequested, this,
            [&](Task *task) { m_task_manager->removeTask(task->uuid()); });
    updateNoTasksLabel();
}

void TasksWidget::removeTaskCard(Task *task) noexcept {
    if (m_task_cards.contains(task->uuid())) {
        auto card = m_task_cards[task->uuid()];
        m_task_cards.remove(task->uuid());
        delete card;
    }

    updateNoTasksLabel();
}
