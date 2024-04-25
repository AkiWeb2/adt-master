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

#ifndef MAINTOOLSWIDGET_H
#define MAINTOOLSWIDGET_H

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QWidget>

#include "interfaces/toolswidgetinterface.h"

namespace Ui
{
class MainToolsWidget;
}

class MainToolsWidget : public QWidget, public ToolsWidgetInterface
{
    Q_OBJECT
public:
    MainToolsWidget(QWidget *parent = nullptr);
    ~MainToolsWidget();

public:
    void setController(MainWindowControllerInterface *controller) override;

    void setModel(QAbstractItemModel *model) override;

    void enableButtons() override;
    void disableButtons() override;

    void enableExitButton() override;
    void disableExitButton() override;

    void setDescription(QString description) override;

    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_runAllPushButton_clicked();

    void on_browseCheckPushButton_clicked();

    void on_exitPushButton_clicked();

    void onSelectionChanged(const QItemSelection &newSelection, const QItemSelection &previousSelection);

private:
    Ui::MainToolsWidget *ui;

    MainWindowControllerInterface *m_controller;

private:
    MainToolsWidget(const MainToolsWidget &) = delete;
    MainToolsWidget(MainToolsWidget &&)      = delete;
    MainToolsWidget &operator=(const MainToolsWidget &) = delete;
    MainToolsWidget &operator=(MainToolsWidget &&) = delete;
};

#endif // MAINTOOLSWIDGET_H
