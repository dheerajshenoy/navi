#include "Task.hpp"

Task::Task() {
    m_uuid = QUuid::createUuid();
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

bool Task::stop() noexcept {}

void Task::runCommand() noexcept {
    QProcess *process = new QProcess(this);

    process->setProgram(m_command);
    if (!m_command_args_list.isEmpty()) {
      process->setArguments(m_command_args_list);
    }

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

    connect(process, &QProcess::readyReadStandardOutput, this, [&]() {
        qDebug() << process->readAllStandardOutput();
    });

    connect(process, &QProcess::finished, this, [&]() {
        qDebug() << "finished";
        emit finished(uuid());
        // process->close();
    });

    // process->waitForFinished(-1);
    // process->waitForFinished();
    process->start();

}