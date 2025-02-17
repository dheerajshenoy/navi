// Task card template for COMMAND command type for viewing the task in the TasksWidget
#pragma once
#include "pch/pch_taskcardtemplate.hpp"
#include "Task.hpp"

class TaskCardTemplate : public QWidget {
    Q_OBJECT
  public:
    struct TaskInfo {
        QString command;
        Task::TaskType type;
    };

    explicit TaskCardTemplate(const QUuid &id, const QString &cmd,
                              const Task::TaskType &type,
                              QWidget *parent = nullptr);

    void setID(const QUuid &id) noexcept;
    void setCommandString(const QString &commandString) noexcept;
    void setCommandType(const Task::TaskType &type) noexcept;
    void appendOutput(const QString &output) noexcept;
    void cancelTask() noexcept;

    inline void setTaskFinished() noexcept {
        m_statusLabel->setText("Status: FINISHED");
        cancelBtn->setEnabled(false);
        removeTaskBtn->setEnabled(true);
    }

    signals:
    void taskCancelRequested(QUuid uuid);
    void taskRemoveRequested(QUuid uuid);
    void progressChanged(const float &progress_value);

  private:
    QUuid m_id;
    QTextEdit *m_outputEdit = nullptr;
    QPushButton *cancelBtn = new QPushButton("Cancel");
    QPushButton *removeTaskBtn = new QPushButton("Remove");
    unsigned int m_command_output_threshold = 4;
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLineEdit *m_commandEdit = new QLineEdit();
    QLabel *m_typeLabel = new QLabel();
    QLabel *m_statusLabel = new QLabel();
    QProgressBar *progressBar = new QProgressBar();
    QPushButton *expandBtn = new QPushButton("Details", this);

};
