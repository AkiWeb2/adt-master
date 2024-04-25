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

#include "maintestswidget.h"
#include "ui_maintestswidget.h"

#include <QStyle>
#include <QThread>

const int LAYOUT_STRETCH_INDEX  = 100;
const int LAYOUT_STRETCH_FACTOR = 400;
const int LAYOUT_INDEX          = 10;

MainTestsWidget::MainTestsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainTestsWidget)
    , m_controller(nullptr)
    , m_summaryLayout(new QVBoxLayout())
    , m_detailsLayout(new QVBoxLayout())
    , m_detailsText(new QPlainTextEdit())
    , m_backToSummaryWidgetButton(new QPushButton())
    , m_helper(nullptr)
    , m_statusWidgets()
{
    ui->setupUi(this);

    ui->summaryScrollAreaWidgetContents->setLayout(m_summaryLayout);
    ui->detailsScrollAreaWidgetContents->setLayout(m_detailsLayout);

    m_backToSummaryWidgetButton->setText(tr("Back"));

    QHBoxLayout *detailsHButtonLayout = new QHBoxLayout();
    detailsHButtonLayout->addStretch();
    detailsHButtonLayout->addWidget(m_backToSummaryWidgetButton);
    connect(m_backToSummaryWidgetButton, &QPushButton::clicked, this, &MainTestsWidget::onBackToSummaryButtonClicked);

    m_detailsLayout->addWidget(m_detailsText);
    m_detailsLayout->insertLayout(LAYOUT_INDEX, detailsHButtonLayout);
    ui->detailsScrollAreaWidgetContents->setLayout(m_detailsLayout);
}

MainTestsWidget::~MainTestsWidget()
{
    delete ui;
}

void MainTestsWidget::setController(MainWindowControllerInterface *controller)
{
    m_controller = controller;
}

void MainTestsWidget::setToolObjectHelper(ADTToolObjectHelper *helper)
{
    m_helper = helper;

    clearUi();

    updateStatusWidgets();

    updateListOfStatusCommonWidgets();
}

void MainTestsWidget::enableButtons()
{
    ui->backPushButton->setEnabled(true);
    ui->exitPushButton->setEnabled(true);
    ui->runAllTestPushButton->setEnabled(true);
    ui->reportButton->setEnabled(true);
    ui->checkfilter->setEnabled(true);
    disableRunTestByDoubleClick(false);
}

void MainTestsWidget::disableButtons()
{
    ui->backPushButton->setEnabled(false);
    ui->exitPushButton->setEnabled(false);
    ui->runAllTestPushButton->setEnabled(false);
    ui->reportButton->setEnabled(false);
    ui->checkfilter->setEnabled(false);
    disableRunTestByDoubleClick(true);
}

void MainTestsWidget::setEnabledRunButtonOfStatusWidgets(bool isEnabled)
{
    std::for_each(m_statusWidgets.begin(), m_statusWidgets.end(), [isEnabled] (StatusCommonWidget *widget) {widget->setEnabledRunButton(isEnabled);});
}

void MainTestsWidget::showDetails(QString detailsText)
{
    m_detailsText->clear();
    m_detailsText->appendPlainText(detailsText);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainTestsWidget::showAllTest()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainTestsWidget::setWidgetStatus(ADTExecutable *task, StatusCommonWidget::WidgetStatus status, bool moveScroll)
{
    StatusCommonWidget *currentWidget = findWidgetByTask(task);

    if (!currentWidget)
    {
        return;
    }

    currentWidget->setWidgetStatus(status);

    if (moveScroll)
    {
        ui->summaryScrollArea->ensureWidgetVisible(currentWidget);
    }
}

void MainTestsWidget::on_runAllTestPushButton_clicked()
{
    std::for_each(m_statusWidgets.begin(), m_statusWidgets.end(), [] (StatusCommonWidget *widget) {
        widget->getExecutable()->clearReports();
        widget->getDetailsDialog()->clearDetailsText();
    });

    m_controller->runCurrentToolTest();
}

void MainTestsWidget::on_backPushButton_clicked()
{
    m_controller->backTestsWigdet();
}

void MainTestsWidget::onExitPushButtonClicked()
{
    m_controller->exitTestsWidget();
}

void MainTestsWidget::onRunButtonCurrentStatusWidgetClicked(StatusCommonWidget *widget)
{
    //TO DO refactor without selecting logic!!!
    std::vector<ADTExecutable *> runningTests;

    runningTests.push_back(widget->getExecutable());

    widget->getDetailsDialog()->clearDetailsText();
    m_controller->runTestsWidget(runningTests);
}

void MainTestsWidget::onDetailsButtonCurrentStatusWidgetClicked(StatusCommonWidget *widget)
{
    m_controller->detailsCurrentTest(widget);
}

void MainTestsWidget::onBackToSummaryButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainTestsWidget::updateStatusWidgets()
{
    if (!m_helper)
    {
        return;
    }

    m_statusWidgets.clear();

    std::vector<ADTExecutable *> tasks = m_helper->getFilteredTasks();

    if (tasks.empty())
    {
        return;
    }

    for (size_t i = 0; i < tasks.size(); i++)
    {
        tasks.at(i)->m_name;
        tasks.at(i)->m_name.contains(ui->checkfilter->text());
        StatusCommonWidget *currentWidget = new StatusCommonWidget(tasks.at(i));

        connect(currentWidget,
                &StatusCommonWidget::logsButtonClicked,
                this,
                &MainTestsWidget::onDetailsButtonCurrentStatusWidgetClicked);

        connect(currentWidget,
                &StatusCommonWidget::runButtonCLicked,
                this,
                &MainTestsWidget::onRunButtonCurrentStatusWidgetClicked);

        m_statusWidgets.push_back(currentWidget);
    }
}

void MainTestsWidget::updateListOfStatusCommonWidgets()
{
    int i = 0;
    for (auto commonStatusWidget : m_statusWidgets)
    {
        m_summaryLayout->insertWidget(i, commonStatusWidget, Qt::AlignTop);
        i++;
    }

    m_summaryLayout->insertStretch(LAYOUT_STRETCH_INDEX, LAYOUT_STRETCH_FACTOR);
}

void MainTestsWidget::clearUi()
{
    delete ui->summaryScrollAreaWidgetContents;

    ui->summaryScrollAreaWidgetContents = new QWidget();

    ui->summaryScrollArea->setWidget(ui->summaryScrollAreaWidgetContents);

    m_summaryLayout = new QVBoxLayout();
    m_summaryLayout->setAlignment(Qt::AlignTop);

    ui->summaryScrollAreaWidgetContents->setLayout(m_summaryLayout);

    ui->stackedWidget->setCurrentIndex(0);
}

StatusCommonWidget *MainTestsWidget::findWidgetByTask(ADTExecutable *task)
{
    for (auto currentWidget : m_statusWidgets)
    {
        if (task == currentWidget->getExecutable())
        {
            return currentWidget;
        }
    }

    qWarning() << "ERROR: can't find status widget by task!";

    return nullptr;
}

void MainTestsWidget::disableRunTestByDoubleClick(bool flag)
{
    for (auto currentWidget : m_statusWidgets)
    {
        currentWidget->disableRunTestByDoubleClick(flag);
    }
}

void MainTestsWidget::on_exitPushButton_clicked()
{
    m_controller->exitTestsWidget();
}

void MainTestsWidget::on_checkfilter_textChanged(const QString &arg1)
{
    m_helper->setFilter(arg1);

    clearUi();

    updateStatusWidgets();

    updateListOfStatusCommonWidgets();
}

void MainTestsWidget::on_clearFilterPushButton_clicked()
{
    ui->checkfilter->clear();
}

void MainTestsWidget::on_reportButton_clicked()
{
    m_controller->reportTestWidget();
}
