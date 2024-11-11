#include "Task.hpp"

Task::Task() : m_uuid(QUuid::createUuid()), process(new QProcess(this)) {
}

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

    // QTemporaryFile tmpFile;
    // if (!tmpFile.open()) {
    //     emit errorOccured("Could not create a temporary file to read the command output");
    //     return;
    // }

    // auto fileName = tmpFile.fileName();
    // m_process->setStandardOutputFile(fileName, QIODevice::Append);
    // QFileSystemWatcher *watcher = new QFileSystemWatcher();
    // watcher->addPath(fileName);
    // connect(watcher, &QFileSystemWatcher::fileChanged, this,
    //         [&](const QString &fileName) {
    // });

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