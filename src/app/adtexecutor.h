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

#ifndef ADTEXECUTOR_H
#define ADTEXECUTOR_H

#include "mainwindow/statuscommonwidget.h"

#include <QDBusInterface>
#include <QObject>

class ADTExecutorPrivate;

class ADTExecutor : public QObject
{
    Q_OBJECT

public:
    ADTExecutor();

    ~ADTExecutor();

    int getAmountOfExecutables();

    void cancelTasks();

    void resetStopFlag();

    void wait();

    void resetWaitFlag();

    bool isRunning();

    void setTasks(std::vector<ADTExecutable *> &tasks);

public slots:
    void runTasks();

signals:
    void beginTask(ADTExecutable *currentExecutable);
    void finishTask(ADTExecutable *currentExecutable);

    void allTaskBegin();
    void allTasksFinished();

private:
    void executeTask(ADTExecutable *task);

    void connectTaskSignals(QDBusInterface &iface,
                            ADTExecutable *task,
                            QString stdoutSignalName,
                            QString stderrSignalName);
    void disconnectTaskSignals(QDBusInterface &iface,
                               ADTExecutable *task,
                               QString stdoutSignalName,
                               QString stderrSignalName);

private:
    ADTExecutorPrivate *d;

private:
    ADTExecutor(const ADTExecutor &) = delete;
    ADTExecutor(ADTExecutor &&)      = delete;
    ADTExecutor &operator=(const ADTExecutor &) = delete;
    ADTExecutor &operator=(ADTExecutor &&) = delete;
};

#endif // ADTEXECUTOR_H
