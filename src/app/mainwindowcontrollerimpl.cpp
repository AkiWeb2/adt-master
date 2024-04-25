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

#include "mainwindowcontrollerimpl.h"
#include "categoryproxymodel.h"
#include "mainwindow/detailsdialog.h"
#include "mainwindow/mainwindow.h"
#include "mainwindow/serviceunregisteredwidget.h"
#include "treeproxymodel.h"

#include <fstream>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>

class MainWindowControllerImplPrivate
{
public:
    MainWindowControllerImplPrivate(TreeModel *model,
                                    QDBusConnection conn,
                                    InterfaceData ifaceData,
                                    ADTSettingsInterface *settings,
                                    CommandLineOptions *options,
                                    QApplication *app)
        : m_model(model)
        , m_dbusConnection(conn)
        , m_interfaceData(ifaceData)
        , m_settings(settings)
        , m_mainWindow(nullptr)
        , m_toolsWidget(nullptr)
        , m_testWidget(nullptr)
        , m_helpers()
        , m_currentTool(nullptr)
        , m_serviceUnregisteredWidget(new ServiceUnregisteredWidget())
        , m_executor(new ADTExecutor())
        , m_workerThread(nullptr)
        , m_isWorkingThreadActive(false)
        , m_options(options)
        , m_application(app)
        , m_proxyModel(new QSortFilterProxyModel())

    {
        m_mainWindow  = new MainWindow();
        m_toolsWidget = m_mainWindow->getToolsWidget();
        m_testWidget  = m_mainWindow->getTestWidget();
        m_serviceUnregisteredWidget->setWindowFlags(Qt::FramelessWindowHint);
    }
    ~MainWindowControllerImplPrivate()
    {
        delete m_proxyModel;
        delete m_serviceUnregisteredWidget;
        delete m_mainWindow;
    }

    TreeModel *m_model;

    QDBusConnection m_dbusConnection;

    InterfaceData m_interfaceData;

    ADTSettingsInterface *m_settings;

    MainWindowInterface *m_mainWindow;

    ToolsWidgetInterface *m_toolsWidget;

    TestWidgetInterface *m_testWidget;

    std::vector<std::unique_ptr<ADTToolObjectHelper>> m_helpers;

    ADTToolObjectHelper *m_currentTool;

    ServiceUnregisteredWidget *m_serviceUnregisteredWidget;

    std::unique_ptr<ADTExecutor> m_executor;

    QThread *m_workerThread;

    bool m_isWorkingThreadActive;

    CommandLineOptions *m_options;

    QApplication *m_application;

    QSortFilterProxyModel *m_proxyModel;

private:
    MainWindowControllerImplPrivate(const MainWindowControllerImplPrivate &) = delete;
    MainWindowControllerImplPrivate(MainWindowControllerImplPrivate &&)      = delete;
    MainWindowControllerImplPrivate &operator=(const MainWindowControllerImplPrivate &) = delete;
    MainWindowControllerImplPrivate &operator=(MainWindowControllerImplPrivate &&) = delete;
};

MainWindowControllerImpl::MainWindowControllerImpl(TreeModel *model,
                                                   QDBusConnection conn,
                                                   InterfaceData ifaceData,
                                                   ADTSettingsInterface *settings,
                                                   CommandLineOptions *options,
                                                   QApplication *app)
    : d(new MainWindowControllerImplPrivate(model, conn, ifaceData, settings, options, app))
{
    d->m_toolsWidget->setController(this);
    d->m_proxyModel->setSourceModel(d->m_model);
    d->m_proxyModel->sort(Qt::DisplayRole);
    d->m_toolsWidget->setModel(d->m_model);
    d->m_toolsWidget->disableButtons();

    buildToolHelpers(d->m_model, d->m_helpers);

    d->m_testWidget->setController(this);

    d->m_mainWindow->setController(this);

    connect(d->m_executor.get(), &ADTExecutor::beginTask, this, &MainWindowControllerImpl::onBeginTask);
    connect(d->m_executor.get(), &ADTExecutor::finishTask, this, &MainWindowControllerImpl::onFinishTask);
    connect(d->m_executor.get(), &ADTExecutor::allTaskBegin, this, &MainWindowControllerImpl::onAllTasksBegin);
    connect(d->m_executor.get(), &ADTExecutor::allTasksFinished, this, &MainWindowControllerImpl::onAllTasksFinished);

    connect(d->m_serviceUnregisteredWidget,
            &ServiceUnregisteredWidget::closeAndExit,
            this,
            &MainWindowControllerImpl::onCloseAndExitButtonPressed);
    connect(d->m_serviceUnregisteredWidget,
            &ServiceUnregisteredWidget::closeAll,
            this,
            &MainWindowControllerImpl::on_closeButtonPressed);
}

MainWindowControllerImpl::~MainWindowControllerImpl()
{
    delete d;
}

void MainWindowControllerImpl::runAllToolsWidget()
{
    d->m_mainWindow->toggleStackWidget();
    runTestsWidget(d->m_currentTool->getFilteredTasks());
}

void MainWindowControllerImpl::chooseToolsWidget()
{
    d->m_mainWindow->toggleStackWidget();
}

void MainWindowControllerImpl::exitToolsWidget()
{
    d->m_mainWindow->closeAll();
}

void MainWindowControllerImpl::changeSelectedTool(QModelIndex index)
{
    TreeItem *item = getTreeItemByIndex(index);

    if (!item)
    {
        return;
    }

    d->m_toolsWidget->setDescription(item->getExecutable()->m_description);

    d->m_toolsWidget->enableButtons();

    for (size_t i = 0; i < d->m_helpers.size(); i++)
    {
        if (d->m_helpers.at(i)->getId() == item->getExecutable()->m_id)
        {
            d->m_testWidget->setToolObjectHelper(d->m_helpers.at(i).get());
            d->m_currentTool = d->m_helpers.at(i).get();
        }
    }
}

TreeItem *MainWindowControllerImpl::getTreeItemByIndex(QModelIndex index)
{
    if (!index.isValid())
    {
        return nullptr;
    }

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    if (!item)
    {
        qWarning() << "ERROR! Can't get TreeItem by index!";
        return nullptr;
    }
    return item;
}

void MainWindowControllerImpl::runTestsWidget(std::vector<ADTExecutable *> tests)
{
    d->m_executor->setTasks(tests);

    d->m_executor->resetStopFlag();

    d->m_workerThread = new QThread();

    connect(d->m_workerThread, &QThread::started, d->m_executor.get(), &ADTExecutor::runTasks);
    connect(d->m_workerThread, &QThread::finished, d->m_workerThread, &QObject::deleteLater);

    d->m_executor->moveToThread(d->m_workerThread);

    d->m_workerThread->start();
}

void MainWindowControllerImpl::runCurrentToolTest()
{
    runTestsWidget(d->m_currentTool->getFilteredTasks());
}

void MainWindowControllerImpl::backTestsWigdet()
{
    d->m_mainWindow->toggleStackWidget();
}

void MainWindowControllerImpl::exitTestsWidget()
{
    if (d->m_executor->isRunning())
    {
        //TO DO show stopping dialog!
        d->m_executor->cancelTasks();
    }
    else
    {
        d->m_mainWindow->closeAll();
    }
}

void MainWindowControllerImpl::reportTestWidget()
{
    if (!d->m_currentTool)
    {
        return;
    }

    QByteArray report = d->m_currentTool->getReport(d->m_dbusConnection);
    if (report.isEmpty())
    {
        QMessageBox warning;
        warning.setText(tr("Can't get report from tool: ") + d->m_currentTool->getId());
        warning.setIcon(QMessageBox::Warning);
        warning.exec();

        return;
    }

    QDate date = QDate::currentDate();

    QString filename(d->m_settings->getReportFilenameTemplate());
    filename.replace("%d", QString::number(date.day()));
    filename.replace("%m", QString::number(date.month()));
    filename.replace("%y", QString::number(date.year()));
    filename.replace("%name", d->m_currentTool->getId());

    QString fileSuffix(d->m_currentTool->getToolTask()->m_reportSuffix);
    QString fullFilename(d->m_settings->getReportPath() + filename);
    QString fileType(fileSuffix + QString(tr(" file")) + QString("(*.") + fileSuffix
                     + QString(tr(");;Any files (*.*)")));

    QString file = QFileDialog::getSaveFileName(nullptr, tr("Save report"), fullFilename, fileType);

    if (file.isEmpty())
    {
        return;
    }

    std::ofstream reportFile;
    reportFile.open(file.toStdString(), std::fstream::out);

    if (reportFile.good())
    {
        reportFile.write(report.data(), report.size());
    }
    else
    {
        QMessageBox warningFile;
        warningFile.setText(tr("Can't save report from tool: ") + d->m_currentTool->getId() + " to file: " + file);
        warningFile.setIcon(QMessageBox::Warning);
        warningFile.exec();
    }
    reportFile.close();
}

void MainWindowControllerImpl::detailsCurrentTest(StatusCommonWidget *widget)
{
    widget->getDetailsDialog()->show();
}

int MainWindowControllerImpl::listObjects()
{
    return 0;
}

int MainWindowControllerImpl::listTestsOfObject(QString object)
{
    QModelIndex toolIndex = getToolById(object);
    if (!toolIndex.isValid())
    {
        qWarning() << "ERROR: can't find tool with id: " + object;
        //TO DO show messagebox with warning
        return 1;
    }

    changeSelectedTool(toolIndex);
    d->m_mainWindow->toggleStackWidget();
    //reportTestWidget();

    return 0;
}

int MainWindowControllerImpl::runAllTestsOfObject(QString object)
{
    QModelIndex toolIndex = getToolById(object);
    if (!toolIndex.isValid())
    {
        qWarning() << "ERROR: can't find tool with id: " + object;
        //TO DO show messagebox with warning
        return 1;
    }

    changeSelectedTool(toolIndex);
    d->m_mainWindow->toggleStackWidget();

    runTestsWidget(d->m_currentTool->getFilteredTasks());
    return 0;
}

int MainWindowControllerImpl::runSpecifiedTestOfObject(QString object, QString test)
{
    QModelIndex toolIndex = getToolById(object);
    if (!toolIndex.isValid())
    {
        qWarning() << "ERROR: can't find tool with id: " + object;
        //TO DO show messagebox with warning
        return 1;
    }

    changeSelectedTool(toolIndex);
    d->m_mainWindow->toggleStackWidget();

    ADTExecutable *testExec = d->m_currentTool->getTestTask(test);

    if (!testExec)
    {
        qWarning() << "ERROR: can't find test with id: " + test + " in tool: " + object;
        //TO DO show messagebox with warning
        return 1;
    }

    runTestsWidget(std::vector<ADTExecutable *>{testExec});
    return 0;
}

int MainWindowControllerImpl::runApp()
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
    default:
        break;
    }

    auto mainWindow = dynamic_cast<MainWindow *>(d->m_mainWindow);

    mainWindow->show();

    d->m_application->exec();

    return result;
}

int MainWindowControllerImpl::getToolReport(QString tool, QString file)
{
    QModelIndex toolIndex = getToolById(tool);

    if (!toolIndex.isValid())
    {
        qWarning() << "ERROR: can't find tool: " + tool;
        //TO DO show messagebox with warning

        return 1;
    }

    changeSelectedTool(toolIndex);
    d->m_mainWindow->toggleStackWidget();
    reportTestWidget();

    return 0;
}

void MainWindowControllerImpl::saveMainWindowSettings()
{
    d->m_settings->saveWindowsSettings(dynamic_cast<QMainWindow *>(d->m_mainWindow));
}

void MainWindowControllerImpl::restoreMainWindowSettings()
{
    d->m_settings->restoreWindowSettings(dynamic_cast<QMainWindow *>(d->m_mainWindow));
}

void MainWindowControllerImpl::on_serviceUnregistered()
{
    if (d->m_executor->isRunning())
    {
        d->m_executor->wait();
    }
    d->m_serviceUnregisteredWidget->show();
    d->m_serviceUnregisteredWidget->startAnimation();
}

void MainWindowControllerImpl::on_serviceRegistered()
{
    if (d->m_serviceUnregisteredWidget->isVisible())
    {
        d->m_serviceUnregisteredWidget->close();
    }

    if (d->m_executor->isRunning())
    {
        d->m_executor->resetWaitFlag();
    }
}

void MainWindowControllerImpl::on_serviceOwnerChanged() {}

void MainWindowControllerImpl::clearAllReports()
{
    QModelIndex rootIndex = d->m_model->parent(QModelIndex());

    TreeItem *rootItem = static_cast<TreeItem *>(rootIndex.internalPointer());

    if (!rootItem)
    {
        qWarning() << "ERROR: can't get root item to clear all reports!";

        return;
    }

    for (int i = rootItem->childCount() - 1; i >= 0; i--)
    {
        clearToolReports(rootItem->child(i));
    }
}

void MainWindowControllerImpl::clearToolReports(TreeItem *item)
{
    if (item->childCount() == 0)
    {
        return;
    }

    for (int i = item->childCount() - 1; i >= 0; i--)
    {
        TreeItem *currentChild = item->child(i);

        currentChild->getExecutable()->clearReports();
    }
}

void MainWindowControllerImpl::onAllTasksBegin()
{
    d->m_isWorkingThreadActive = true;
    d->m_testWidget->setEnabledRunButtonOfStatusWidgets(false);
    d->m_testWidget->disableButtons();
}

void MainWindowControllerImpl::onAllTasksFinished()
{
    d->m_isWorkingThreadActive = false;
    d->m_testWidget->setEnabledRunButtonOfStatusWidgets(true);
    d->m_testWidget->enableButtons();
}

void MainWindowControllerImpl::onBeginTask(ADTExecutable *task)
{
    d->m_testWidget->setWidgetStatus(task, StatusCommonWidget::WidgetStatus::running);
}

void MainWindowControllerImpl::onFinishTask(ADTExecutable *task)
{
    if (task->m_exit_code == 0)
    {
        d->m_testWidget->setWidgetStatus(task, StatusCommonWidget::WidgetStatus::finishedOk);
    }
    else
    {
        d->m_testWidget->setWidgetStatus(task, StatusCommonWidget::WidgetStatus::finishedFailed);
    }
}

void MainWindowControllerImpl::onCloseAndExitButtonPressed()
{
    d->m_executor->cancelTasks();
    d->m_executor->resetWaitFlag();
    d->m_serviceUnregisteredWidget->close();
    d->m_mainWindow->closeAll();
}

void MainWindowControllerImpl::on_closeButtonPressed()
{
    d->m_executor->cancelTasks();
    d->m_executor->resetWaitFlag();
    d->m_serviceUnregisteredWidget->close();
}

QModelIndex MainWindowControllerImpl::getToolById(QString id)
{
    QModelIndex root = d->m_model->parent(QModelIndex());

    for (int i = 0; i < d->m_model->rowCount(root); i++)
    {
        QModelIndex child = d->m_model->index(i, 0, root);

        if (!child.isValid())
        {
            break;
        }

        TreeItem *item = static_cast<TreeItem *>(child.internalPointer());

        if (item->getExecutable()->m_id == id)
        {
            return child;
        }
    }

    return QModelIndex();
}
