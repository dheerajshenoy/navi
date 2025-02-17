#include "TaskCardTemplate.hpp"

TaskCardTemplate::TaskCardTemplate(const QUuid &id, const QString &cmd,
                                   const Task::TaskType &type,
                                   QWidget *parent) : m_id(id) {

    setLayout(layout);

    m_commandEdit->setPlaceholderText(cmd);
    m_commandEdit->setReadOnly(true);

    expandBtn->setCheckable(true);
    expandBtn->setChecked(true);

    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->addWidget(removeTaskBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(expandBtn, 1);

    removeTaskBtn->setEnabled(false);

    connect(removeTaskBtn, &QPushButton::clicked, this, [&]() {
        emit taskRemoveRequested(m_id);
    });

    connect(expandBtn, &QPushButton::clicked, this,
            [&](const bool &state) { m_outputEdit->setVisible(state); });

    layout->addWidget(m_commandEdit);

    QHBoxLayout *type_n_status_layout = new QHBoxLayout();
    type_n_status_layout->addWidget(m_typeLabel);
    type_n_status_layout->addWidget(m_statusLabel);

    m_statusLabel->setText("Status: RUNNING");

    layout->addLayout(type_n_status_layout);

    // If it's a CommandTask, add a QTextEdit for real-time output
    switch (type) {
        case Task::TaskType::COMMAND: {

            // Display Task Type
            QString taskType = "COMMAND";

            m_typeLabel->setText("Type: " + taskType);

            m_outputEdit = new QTextEdit();
            m_outputEdit->setReadOnly(true);
            layout->addWidget(m_outputEdit);

            connect(cancelBtn, &QPushButton::clicked, this, [&]() {
                auto reply = QMessageBox::question(this, "Cancel task", "Do you want to cancel this task ?");
                if (reply == QMessageBox::StandardButton::Yes) {
                    cancelBtn->setEnabled(false);
                    removeTaskBtn->setEnabled(true);
                    emit taskCancelRequested(m_id);
                }
            });
        }
            break;

        case Task::TaskType::COPY: {
            connect(cancelBtn, &QPushButton::clicked, this, [&]() {
                auto reply = QMessageBox::question(this, "Cancel task",
                                                   "Do you want to cancel this task ?");
                if (reply == QMessageBox::StandardButton::Yes) {
                    cancelBtn->setEnabled(false);
                    removeTaskBtn->setEnabled(true);
                    emit taskCancelRequested(m_id);
                }
            });

            // Display Task Type
            QString taskType = "COPY";

            m_typeLabel->setText("Type: " + taskType);

            connect(this, &TaskCardTemplate::progressChanged, this,
                    [&](const float &value) {
                        progressBar->setValue(value);
                    });

            QFormLayout *formLayout = new QFormLayout();
            formLayout->addRow("Progress: ", progressBar);
            layout->addLayout(formLayout);
        } break;

        case Task::TaskType::MOVE:
        case Task::TaskType::DELETE:
        case Task::TaskType::TRASH:
            // TODO: Moving, Deleting and Trashing task progress
            break;
    }

    layout->addLayout(btnLayout);
}

void TaskCardTemplate::appendOutput(const QString &output) noexcept {
    if (m_outputEdit) {
        if (m_outputEdit->document()->lineCount() > m_command_output_threshold)
            m_outputEdit->clear();
        m_outputEdit->append(output);
    }
}

void TaskCardTemplate::setCommandString(const QString &cmd) noexcept {
    m_commandEdit->setPlaceholderText(cmd);
}

void TaskCardTemplate::setCommandType(const Task::TaskType &type) noexcept {

    switch (type) {
        case Task::TaskType::COMMAND:
            m_typeLabel->setText("COMMAND");
            break;

        case Task::TaskType::COPY:
            m_typeLabel->setText("COPY");
            break;

        case Task::TaskType::DELETE:
            m_typeLabel->setText("DELETE");
            break;


        case Task::TaskType::MOVE:
            m_typeLabel->setText("MOVE");
            break;

        case Task::TaskType::TRASH:
            m_typeLabel->setText("TRASH");
            break;
    }
}
