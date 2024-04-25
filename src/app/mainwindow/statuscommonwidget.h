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

#ifndef STATUSCOMMONWIDGET_H
#define STATUSCOMMONWIDGET_H

#include "detailsdialog.h"

#include <../core/treeitem.h>
#include <QWidget>

namespace Ui
{
class StatusCommonWidget;
}

class StatusCommonWidget : public QWidget
{
    Q_OBJECT

    struct WidgetBackgroundColor
    {
        WidgetBackgroundColor() {}
        WidgetBackgroundColor(int r, int g, int b)
        {
            red   = r;
            green = g;
            blue  = b;
        }
        int red   = 0;
        int green = 0;
        int blue  = 0;
    };

    struct TestOkColor : public WidgetBackgroundColor
    {
        TestOkColor()
            : WidgetBackgroundColor(253, 253, 253)
        {}
    };

    struct TestFailedColor : public WidgetBackgroundColor
    {
        TestFailedColor()
            : WidgetBackgroundColor(186, 189, 182)
        {}
    };

    struct TestReadyColor : public WidgetBackgroundColor
    {
        TestReadyColor()
            : WidgetBackgroundColor(186, 189, 182)
        {}
    };

    struct TestRunningColor : public WidgetBackgroundColor
    {
        TestRunningColor()
            : WidgetBackgroundColor(181, 199, 164)
        {}
    };

public:
    enum WidgetStatus
    {
        ready,
        running,
        finishedOk,
        finishedFailed
    };

public:
    StatusCommonWidget(ADTExecutable *exec, QWidget *parent = nullptr);

    ~StatusCommonWidget();

    ADTExecutable *getExecutable();

    void setText(QString text);

    void setIcon(QIcon &icon);

    void setEnabledRunButton(bool isEnabled);

    void setWidgetStatus(StatusCommonWidget::WidgetStatus status);

    void disableRunTestByDoubleClick(bool flag);

    DetailsDialog *getDetailsDialog();

signals:
    void logsButtonClicked(StatusCommonWidget *widget);

    void runButtonCLicked(StatusCommonWidget *widget);

private slots:
    void on_runPushButton_clicked();

    void on_logsPushButton_clicked();

private:
    Ui::StatusCommonWidget *ui;

    ADTExecutable *executable;

    DetailsDialog *detailsDialog;

    QColor m_defaultColor;

private:
    StatusCommonWidget(const StatusCommonWidget &) = delete;
    StatusCommonWidget(StatusCommonWidget &&)      = delete;
    StatusCommonWidget &operator=(const StatusCommonWidget &) = delete;
    StatusCommonWidget &operator=(StatusCommonWidget &&) = delete;
};

#endif // STATUSCOMMONWIDGET_H
