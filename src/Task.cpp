#include "Task.hpp"

Task::Task(QObject *parent)
    : m_uuid(QUuid::createUuid()), process(new QProcess(parent)) {}

void Task::setTaskType(const TaskType &type) noexcept { m_type = type; }

void Task::run() noexcept {
    switch (m_type) {

    case TaskType::COMMAND:
        runCommand();
        break;

    case TaskType::COPY:
    case TaskType::MOVE:
    case TaskType::DELETE:
       break;
    }
}

void Task::setCommandString(const QString &command,
                            const QStringList &args) noexcept {
    m_command = command;
    m_command_args_list = args;
}

void Task::stop() noexcept {
    if (process->state() != QProcess::NotRunning) {
        process->terminate();
        if (!process->waitForFinished(3000)) {
            process->kill();
        }
    }
}

void Task::runCommand() noexcept {

    connect(process, &QProcess::readyReadStandardOutput, this,
            [&]() { emit stdout(process->readAllStandardOutput()); });

    connect(process, &QProcess::readyReadStandardError, this, [&]() {
        emit stderr(process->readAllStandardError());
    });

    connect(process, &QProcess::finished, this, [&]() {
      emit finished(uuid());
      // process->close();
    });

    process->waitForFinished(-1);
    process->waitForReadyRead(-1);

    process->start(m_command, m_command_args_list);

}

void Task::cancel() noexcept {
    process->terminate();
}
