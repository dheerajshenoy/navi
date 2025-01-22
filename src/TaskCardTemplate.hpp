// Task card template for COMMAND command type for viewing the task in the TasksWidget
#pragma once

#include "pch/pch_taskcardtemplate.hpp"

#include "Task.hpp"

class TaskCardTemplate : public QWidget {
    Q_OBJECT
public:
  explicit TaskCardTemplate(Task *task, QWidget *parent = nullptr)
      : QWidget(parent), task(task) {

    setLayout(layout);

    commandEdit->setPlaceholderText(task->commandString());
    commandEdit->setReadOnly(true);
    layout->addWidget(commandEdit);

    layout->addWidget(typeLabel);

    QPushButton *expandBtn = new QPushButton("Expand", this);
    expandBtn->setCheckable(true);
    expandBtn->setChecked(true);

    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(expandBtn);
    layout->addLayout(btnLayout);
    connect(expandBtn, &QPushButton::clicked, this,
            [&](const bool &state) { outputEdit->setVisible(state); });


    // If it's a CommandTask, add a QTextEdit for real-time output
    switch (task->type()) {
    case Task::TaskType::COMMAND: {

        // Display Task Type
        QString taskType = "COMMAND";

        typeLabel->setText("Type: " + taskType);

        outputEdit = new QTextEdit();
        outputEdit->setReadOnly(true);
        layout->addWidget(outputEdit);

        // Connect output signal to update QTextEdit
        connect(task, &Task::stdout, this, &TaskCardTemplate::appendOutput);

        connect(cancelBtn, &QPushButton::clicked, task, [this, task]() {
            auto reply = QMessageBox::question(
                                               this, "Cancel task", "Do you want to cancel this task ?");
            if (reply == QMessageBox::StandardButton::Yes) {
                emit taskCancelRequested(task);
            }
        });
    }
        break;

    case Task::TaskType::COPY: {
      connect(cancelBtn, &QPushButton::clicked, task, [this, task]() {
        auto reply = QMessageBox::question(this, "Cancel task",
                                           "Do you want to cancel this task ?");
        if (reply == QMessageBox::StandardButton::Yes) {
          emit taskCancelRequested(task);
        }
      });

      // Display Task Type
      QString taskType = "COPY";

      typeLabel->setText("Type: " + taskType);

      connect(this, &TaskCardTemplate::progressChanged, this,
              [&](const float &value) {
                  progressBar->setValue(value);
                });

      formLayout->addRow("Progress: ", progressBar);
      layout->addLayout(formLayout);
    } break;

    case Task::TaskType::MOVE:
    case Task::TaskType::DELETE:
    case Task::TaskType::TRASH:
      break;
    }

}

signals:
    void taskCancelRequested(Task *task);
    void progressChanged(const float &progress_value);

public slots:
void appendOutput(const QString &output) {
    if (outputEdit) {
        if (outputEdit->document()->lineCount() > m_command_output_threshold)
            outputEdit->clear();
        outputEdit->append(output);
    }
}

private:
    Task *task;
    QTextEdit *outputEdit = nullptr;
    QPushButton *cancelBtn = new QPushButton("Cancel");
    unsigned int m_command_output_threshold = 4;
    QFormLayout *formLayout = new QFormLayout();
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLineEdit *commandEdit = new QLineEdit();
    QLabel *typeLabel = new QLabel();
    QProgressBar *progressBar = new QProgressBar();

};
