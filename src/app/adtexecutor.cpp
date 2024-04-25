/***********************************************************************************************************************
**
** Copyright (C) 2023 BaseALT Ltd. <org@basealt.ru>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
***********************************************************************************************************************/

#include "adtexecutor.h"

#include <QApplication>
#include <QDBusConnection>
#include <QDBusReply>
#include <QThread>
#include <QTimer>
#include <qdbusmessage.h>

const QString STDOUT_SIGNAL_NAME = "diag1_stdout_signal";
const QString STDERR_SIGNAL_NAME = "diag1_stderr_signal";

class ADTExecutorPrivate
{
public:
    ADTExecutorPrivate()
        : executables()
        , stopFlag(false)
        , waitFlag(false)
        , isRunning(false)
    {}

    ~ADTExecutorPrivate() {}

    std::vector<ADTExecutable *> executables;

    volatile bool stopFlag;
    volatile bool waitFlag;
    volatile bool isRunning;

private:
    ADTExecutorPrivate(const ADTExecutorPrivate &) = delete;
    ADTExecutorPrivate(ADTExecutorPrivate &&)      = delete;
    ADTExecutorPrivate &operator=(const ADTExecutorPrivate &) = delete;
    ADTExecutorPrivate &operator=(ADTExecutorPrivate &&) = delete;
};

ADTExecutor::ADTExecutor()
    : d(new ADTExecutorPrivate)
{}

ADTExecutor::~ADTExecutor()
{
    delete d;
}

int ADTExecutor::getAmountOfExecutables()
{
    return d->executables.size();
}

void ADTExecutor::cancelTasks()
{
    d->stopFlag = true;
}

void ADTExecutor::resetStopFlag()
{
    d->stopFlag = false;
}

void ADTExecutor::wait()
{
    d->waitFlag = true;
}

void ADTExecutor::resetWaitFlag()
{
    d->waitFlag = false;
}

bool ADTExecutor::isRunning()
{
    return d->isRunning;
}

void ADTExecutor::setTasks(std::vector<ADTExecutable *> &tasks)
{
    d->executables.clear();

    d->executables.assign(tasks.begin(), tasks.end());
}

void ADTExecutor::runTasks()
{
    emit allTaskBegin();

    int tasksCount = d->executables.size();

    if (tasksCount == 0)
    {
        this->moveToThread(QApplication::instance()->thread());

        emit allTasksFinished();

        QThread::currentThread()->quit();

        return;
    }

    d->isRunning = true;

    for (ADTExecutable *executable : d->executables)
    {
        if (d->stopFlag)
        {
            break;
        }

        if (d->waitFlag)
        {
            while (true)
            {
                if (d->waitFlag)
                {
                    QThread::currentThread()->yieldCurrentThread();
                }
                else
                {
                    break;
                }
            }
        }

        emit beginTask(executable);

        executeTask(executable);

        emit finishTask(executable);
    }
    d->isRunning = false;

    this->moveToThread(QApplication::instance()->thread());

    emit allTasksFinished();

    QThread::currentThread()->quit();
}

void ADTExecutor::executeTask(ADTExecutable *task)
{
    QDBusConnection dbus(QDBusConnection::systemBus());

    QDBusInterface dbusIface(task->m_dbusServiceName, task->m_dbusPath, task->m_dbusInterfaceName, dbus);

    task->clearReports();

    QString signalPrefix = dbus.baseService();
    signalPrefix.replace(':', '_');
    signalPrefix.replace('.', '_');

    QString stdoutSignal = STDOUT_SIGNAL_NAME + signalPrefix;
    QString stderrSignal = STDERR_SIGNAL_NAME + signalPrefix;

    connectTaskSignals(dbusIface, task, stdoutSignal, stderrSignal);

    QDBusReply<int> reply = dbusIface.call(task->m_dbusRunMethodName, task->m_id);

    if (!reply.isValid())
    {
        task->m_exit_code = -1;
        task->getStderr(reply.error().message());
        return;
    }

    disconnectTaskSignals(dbusIface, task, stdoutSignal, stderrSignal);

    task->m_exit_code = reply.value();
}

void ADTExecutor::connectTaskSignals(QDBusInterface &iface,
                                     ADTExecutable *task,
                                     QString stdoutSignalName,
                                     QString stderrSignalName)
{
    iface.connection().connect(task->m_dbusServiceName,
                               task->m_dbusPath,
                               task->m_dbusInterfaceName,
                               stdoutSignalName,
                               task,
                               SLOT(getStdout(QString)));

    iface.connection().connect(task->m_dbusServiceName,
                               task->m_dbusPath,
                               task->m_dbusInterfaceName,
                               stderrSignalName,
                               task,
                               SLOT(getStderr(QString)));
}

void ADTExecutor::disconnectTaskSignals(QDBusInterface &iface,
                                        ADTExecutable *task,
                                        QString stdoutSignalName,
                                        QString stderrSignalName)
{
    iface.connection().disconnect(task->m_dbusServiceName,
                                  task->m_dbusPath,
                                  task->m_dbusInterfaceName,
                                  stdoutSignalName,
                                  task,
                                  SLOT(getStdout(QString)));

    iface.connection().disconnect(task->m_dbusServiceName,
                                  task->m_dbusPath,
                                  task->m_dbusInterfaceName,
                                  stderrSignalName,
                                  task,
                                  SLOT(getStderr(QString)));
}
