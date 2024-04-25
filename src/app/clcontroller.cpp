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

#include "clcontroller.h"
#include "../core/treeitem.h"
#include "adtexecutor.h"

#include <fstream>
#include <iostream>

class CLControllerPrivate
{
public:
    CLControllerPrivate(TreeModel *model,
                        QDBusConnection conn,
                        InterfaceData ifaceData,
                        ADTSettingsInterface *settings,
                        CommandLineOptions *options)
        : m_options(options)
        , m_model(model)
        , m_dbusConnection(conn)
        , m_interfaceData(ifaceData)
        , m_helpers()
        , m_settings(settings)
        , m_executor(new ADTExecutor())
    {}
    ~CLControllerPrivate() { delete m_executor; }

    CommandLineOptions *m_options;
    TreeModel *m_model;
    QDBusConnection m_dbusConnection;
    InterfaceData m_interfaceData;
    std::vector<std::unique_ptr<ADTToolObjectHelper>> m_helpers;
    ADTSettingsInterface *m_settings;
    ADTExecutor *m_executor;

private:
    CLControllerPrivate(const CLControllerPrivate &) = delete;
    CLControllerPrivate(CLControllerPrivate &&)      = delete;
    CLControllerPrivate &operator=(const CLControllerPrivate &) = delete;
    CLControllerPrivate &operator=(CLControllerPrivate &&) = delete;
};

CLController::CLController(TreeModel *model,
                           QDBusConnection conn,
                           InterfaceData ifaceData,
                           ADTSettingsInterface *settings,
                           CommandLineOptions *options)
    : d(new CLControllerPrivate(model, conn, ifaceData, settings, options))
{
    buildToolHelpers(d->m_model, d->m_helpers);

    connect(d->m_executor, &ADTExecutor::beginTask, this, &CLController::onBeginTask);
    connect(d->m_executor, &ADTExecutor::finishTask, this, &CLController::onFinishTask);
    connect(d->m_executor, &ADTExecutor::allTaskBegin, this, &CLController::onAllTasksBegin);
    connect(d->m_executor, &ADTExecutor::allTasksFinished, this, &CLController::onAllTasksFinished);
}

CLController::~CLController()
{
    delete d;
}

int CLController::listObjects()
{
    for (auto &helper : d->m_helpers)
    {
        std::cout << helper->getId().toStdString() << std::endl;
    }

    return 0;
}

int CLController::listTestsOfObject(QString object)
{
    ADTToolObjectHelper *tool = getToolById(object);

    if (!tool)
    {
        return 1;
    }

    for (auto test : tool->getAllTasks())
    {
        std::cout << test->m_id.toStdString() << std::endl;
    }

    return 0;
}

int CLController::runAllTestsOfObject(QString object)
{
    ADTToolObjectHelper *tool = getToolById(object);

    if (!tool)
    {
        std::cerr << "ERROR: can't find object: " << object.toStdString() << std::endl;
        return 1;
    }

    std::vector<ADTExecutable *> tests = tool->getAllTasks();
    if (tests.size() == 0)
    {
        std::cerr << "ERROR: can't find tests in object: " << object.toStdString() << std::endl;
        return 2;
    }

    d->m_executor->resetStopFlag();
    d->m_executor->setTasks(tests);
    d->m_executor->runTasks();

    return 0;
}

int CLController::runSpecifiedTestOfObject(QString object, QString test)
{
    ADTToolObjectHelper *tool = getToolById(object);

    if (!tool)
    {
        std::cerr << "ERROR: can't find object: " << object.toStdString() << std::endl;
        return 1;
    }

    std::vector<ADTExecutable *> tests = tool->getAllTasks();
    if (tests.size() == 0)
    {
        std::cerr << "ERROR: can't find tests in object: " << object.toStdString() << std::endl;
        return 2;
    }

    std::vector<ADTExecutable *> tasks;

    for (auto task : tests)
    {
        if (task->m_id == test)
        {
            tasks.push_back(task);

            d->m_executor->resetStopFlag();
            d->m_executor->setTasks(tasks);
            d->m_executor->runTasks();

            return 0;
        }
    }

    std::cerr << "ERROR: can't find test: " << test.toStdString() << " in object: " << object.toStdString()
              << std::endl;

    return 3;
}

int CLController::getToolReport(QString tool, QString file)
{
    ADTToolObjectHelper *toolObject = getToolById(tool);

    if (!toolObject)
    {
        std::cerr << "ERROR: can't find object: " << tool.toStdString() << std::endl;
        return 1;
    }

    QByteArray report = toolObject->getReport(d->m_dbusConnection);

    if (report.isEmpty())
    {
        std::cerr << "ERROR: can't get report from object : " << tool.toStdString() << std::endl;
        return 1;
    }

    std::ofstream reportFile;
    reportFile.open(file.toStdString(), std::fstream::out);

    if (reportFile.good())
    {
        reportFile.write(report.data(), report.size());
    }
    else
    {
        std::cerr << "ERROR: can't save report to file: " << file.toStdString() << std::endl;
        return 1;
    }
    reportFile.close();

    return 0;
}

int CLController::runApp()
{
    int result = -1;

    switch (d->m_options->action)
    {
    case CommandLineOptions::Action::listOfObjects:
        result = listObjects();
        break;
    case CommandLineOptions::Action::listOfTestFromSpecifiedObject:
        result = listTestsOfObject(d->m_options->objectName);
        break;
    case CommandLineOptions::Action::runAllTestFromSpecifiedObject:
        result = runAllTestsOfObject(d->m_options->objectName);
        break;
    case CommandLineOptions::Action::runSpecifiedTestFromSpecifiedObject:
        result = runSpecifiedTestOfObject(d->m_options->objectName, d->m_options->testName);
        break;
    case CommandLineOptions::Action::getReportTool:
        result = getToolReport(d->m_options->objectName, d->m_options->reportFilename);
        break;
    default:
        break;
    }

    return result;
}

void CLController::on_serviceUnregistered()
{
    if (d->m_executor->isRunning())
    {
        std::cout << "Service alterator-manager.service was unregistered! Please, restart the service! Waiting..."
                  << std::endl;
        d->m_executor->wait();
    }
}

void CLController::on_serviceRegistered()
{
    if (d->m_executor->isRunning())
    {
        std::cout << "Service alterator-manager.service was regictered! Working..." << std::endl;
        d->m_executor->resetWaitFlag();
    }
}

void CLController::on_serviceOwnerChanged()
{
    std::cout << "The owner of alterator-manager.service was changed! exiting..." << std::endl;
    if (d->m_executor->isRunning())
    {
        d->m_executor->cancelTasks();
        d->m_executor->resetWaitFlag();
    }
}

ADTToolObjectHelper *CLController::getToolById(QString id)
{
    for (auto &helper : d->m_helpers)
    {
        if (helper->getId() == id)
        {
            return helper.get();
        }
    }

    return nullptr;
}

void CLController::onAllTasksBegin() {}

void CLController::onAllTasksFinished() {}

void CLController::onBeginTask(ADTExecutable *task)
{
    std::cout << "Running test: " << task->m_id.toStdString() << "...";
}

void CLController::onFinishTask(ADTExecutable *task)
{
    if (task->m_exit_code == 0)
    {
        std::cout << "OK" << std::endl;
    }
    else
    {
        std::cout << "ERROR" << std::endl;
    }
}
