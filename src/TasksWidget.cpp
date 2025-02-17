#include "TasksWidget.hpp"
#include <qnamespace.h>

TasksWidget::TasksWidget(TaskManager *taskManager)
: KDDockWidgets::QtWidgets::DockWidget("Tasks"),
    m_task_manager(taskManager) {

    QWidget *widget = new QWidget(this);
    widget->setLayout(m_layout);
    this->setWidget(widget);

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

    connect(m_task_manager, &TaskManager::taskAdded, this, &TasksWidget::addTaskCard);
    connect(m_task_manager, &TaskManager::taskRemoved, this, &TasksWidget::removeTaskCard);


    m_no_tasks_label->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_no_tasks_label);

    updateNoTasksLabel();
}

void TasksWidget::updateNoTasksLabel() noexcept {
    m_no_tasks_label->setVisible(m_task_cards.isEmpty());
    taskContainer->setVisible(!m_task_cards.isEmpty());
}

void TasksWidget::addTaskCard(Task *task) noexcept {
    auto *taskCard = new TaskCardTemplate(task->uuid(), task->commandString(), task->type(), this);

    connect(task, &Task::progress, taskCard, &TaskCardTemplate::progressChanged);

    // Connect output signal to update QTextEdit
    connect(task, &Task::stdout, taskCard, &TaskCardTemplate::appendOutput);

    connect(taskCard, &TaskCardTemplate::taskRemoveRequested, taskCard, [&] (QUuid uuid) {
        m_task_manager->removeTask(uuid);
        this->removeTaskCard(uuid);
    });

    m_task_cards[task->uuid()] = taskCard;
    taskLayout->insertWidget(0, taskCard);
    connect(taskCard, &TaskCardTemplate::taskCancelRequested, this,
            [&](QUuid uuid) {
                m_task_manager->cancelTask(uuid);
                // m_task_manager->removeTask(task->uuid());
            });


    connect(task, &Task::finished, this, [&](QUuid uuid) {
        if (m_task_cards.contains(uuid)) {
            auto card = m_task_cards[uuid];
            card->setTaskFinished();
            m_task_manager->removeTask(uuid);
        }
    });


    updateNoTasksLabel();
}

void TasksWidget::removeTaskCard(const QUuid &uuid) noexcept {
    if (m_task_cards.contains(uuid)) {
        auto card = m_task_cards.take(uuid);
        if (card)
            card->deleteLater();
    }

    updateNoTasksLabel();
}
