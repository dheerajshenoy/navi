#pragma once

#include <QObject>
#include <QString>
#include <QUuid>
#include <QProcess>
#include <QTemporaryFile>
#include <QFileSystemWatcher>
#include <QDebug>

class Task : public QObject {
    Q_OBJECT

  public:
    Task();
    enum class TaskType { COMMAND = 0, COPY, MOVE, DELETE };
    void setTaskType(const TaskType &type) noexcept;
    void setCommandString(const QString &command, const QStringList &args) noexcept;
    void run() noexcept;
    inline QUuid uuid() const noexcept { return m_uuid; }
    bool stop() noexcept;

    signals:
    void progress(const int &);
    void finished(const QUuid &uuid);
    void errorOccured(const QString &reason);

private:
    void runCommand() noexcept;

    QUuid m_uuid;
    TaskType m_type;
    QString m_command;
    QStringList m_command_args_list;
};