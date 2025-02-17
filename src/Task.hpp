#pragma once

#include "pch/pch_task.hpp"

class Task : public QObject {
    Q_OBJECT

  public:
    Task(QObject *parent = nullptr);

    enum class TaskType {
        COMMAND = 0,
        COPY,
        MOVE,
        DELETE,
        TRASH
    };

    enum class TaskStatus {
        RUNNING = 0,
        DONE
    };

    void cancel() noexcept;
    void setTaskType(const TaskType &type) noexcept;
    void setCommandString(const QString &command, const QStringList &args) noexcept;
    void run() noexcept;
    const QUuid uuid() const noexcept { return m_uuid; }
    void stop() noexcept;
    TaskType type() const noexcept { return m_type; }
    QString commandString() const noexcept {
        return m_command + " " + m_command_args_list.join(" ");
    }

    void setProgress(const float &_progress) noexcept {
        m_progress = _progress;
        emit progress(_progress);
    }

    void setFinished(const bool &state) noexcept {
        if (state)
            emit finished(m_uuid);
    }

    signals:
    void progress(const float &);
    void finished(const QUuid &uuid);
    void errorOccured(const QString &reason);
    void stdout(const QString &output);
    void stderr(const QString &error);

private:
    void runCommand() noexcept;
    QUuid m_uuid;
    TaskType m_type;
    QString m_command;
    QStringList m_command_args_list;
    QProcess *process;
    float m_progress;
};
