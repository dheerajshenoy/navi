// Task card template for COMMAND command type for viewing the task in the TasksWidget
#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>

#include "Task.hpp"

class TaskCardTemplate : public QWidget {
    Q_OBJECT
public:
  explicit TaskCardTemplate(Task *task, QWidget *parent = nullptr)
      : QWidget(parent), task(task) {

          QVBoxLayout *layout = new QVBoxLayout(this);

    // QLabel *idLabel = new QLabel("ID: " + task->uuid().toString(),
    // this); layout->addWidget(idLabel);
    QLineEdit *commandEdit = new QLineEdit(this);
    commandEdit->setPlaceholderText(task->commandString());
    commandEdit->setReadOnly(true);
    layout->addWidget(commandEdit);

    // Display Task Type
    QString taskType = "COMMAND";

    QLabel *typeLabel = new QLabel("Type: " + taskType, this);
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
    if (task->type() == Task::TaskType::COMMAND) {
        outputEdit = new QTextEdit(this);
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
    setLayout(layout);
}

signals:
    void taskCancelRequested(Task *task);

public slots:
    void appendOutput(const QString &output) {
    if (outputEdit) {
        outputEdit->append(output);
    }
}

private:
    Task *task;
QTextEdit *outputEdit = nullptr;
QPushButton *cancelBtn = new QPushButton("Cancel");
};