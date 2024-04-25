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

#ifndef MAINTESTSWIDGET_H
#define MAINTESTSWIDGET_H

#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "../core/treemodel.h"
#include "adttoolobjecthelper.h"
#include "interfaces/testswidgetinterface.h"

#include "adtexecutor.h"
#include "statuscommonwidget.h"

namespace Ui
{
class MainTestsWidget;
}

class MainTestsWidget : public QWidget, public TestWidgetInterface
{
    Q_OBJECT
public:
    MainTestsWidget(QWidget *parent = nullptr);
    ~MainTestsWidget();

    void setController(MainWindowControllerInterface *controller) override;

    void setToolObjectHelper(ADTToolObjectHelper *helper) override;

    void enableButtons() override;
    void disableButtons() override;
    virtual void setEnabledRunButtonOfStatusWidgets(bool isEnabled) override;

    void showDetails(QString detailsText) override;

    void showAllTest() override;

    void setWidgetStatus(ADTExecutable *task, StatusCommonWidget::WidgetStatus status, bool moveScroll = true) override;

private slots:
    void on_runAllTestPushButton_clicked();

    void on_backPushButton_clicked();

    void onExitPushButtonClicked();

    void onRunButtonCurrentStatusWidgetClicked(StatusCommonWidget *widget);
    void onDetailsButtonCurrentStatusWidgetClicked(StatusCommonWidget *widget);
    void onBackToSummaryButtonClicked();

    void on_exitPushButton_clicked();

    void on_checkfilter_textChanged(const QString &arg1);

    void on_clearFilterPushButton_clicked();

    void on_reportButton_clicked();

private:
    void updateStatusWidgets();

    void updateListOfStatusCommonWidgets();

    void clearUi();

    StatusCommonWidget *findWidgetByTask(ADTExecutable *task);

    void disableRunTestByDoubleClick(bool flag);

private:
    Ui::MainTestsWidget *ui;

    MainWindowControllerInterface *m_controller;

    QVBoxLayout *m_summaryLayout;
    QVBoxLayout *m_detailsLayout;
    QPlainTextEdit *m_detailsText;
    QPushButton *m_backToSummaryWidgetButton;

    ADTToolObjectHelper *m_helper;

   std::vector<StatusCommonWidget *> m_statusWidgets;

private:
    MainTestsWidget(const MainTestsWidget &) = delete;
    MainTestsWidget(MainTestsWidget &&)      = delete;
    MainTestsWidget &operator=(const MainTestsWidget &) = delete;
    MainTestsWidget &operator=(MainTestsWidget &&) = delete;
};

#endif // MAINTESTSWIDGET_H
