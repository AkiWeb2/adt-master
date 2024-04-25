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

#include "maintoolswidget.h"
#include "ui_maintoolswidget.h"

#include <QDebug>
#include <QMouseEvent>



MainToolsWidget::MainToolsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainToolsWidget)
    , m_controller(nullptr)
{

    ui->setupUi(this);

    ui->toolsListView->viewport()->installEventFilter(this);
}

void MainToolsWidget::setController(MainWindowControllerInterface *controller)
{
    m_controller = controller;
}

MainToolsWidget::~MainToolsWidget()
{
    delete ui;
}

void MainToolsWidget::enableButtons()
{
    ui->runAllPushButton->setEnabled(true);
    ui->browseCheckPushButton->setEnabled(true);
}

void MainToolsWidget::disableButtons()
{
    ui->runAllPushButton->setEnabled(false);
    ui->browseCheckPushButton->setEnabled(false);
}

void MainToolsWidget::enableExitButton()
{
    ui->exitPushButton->setEnabled(true);
}

void MainToolsWidget::disableExitButton()
{
    ui->exitPushButton->setEnabled(false);
}

void MainToolsWidget::setModel(QAbstractItemModel *model)
{
    ui->toolsListView->setModel(model);

    connect(ui->toolsListView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MainToolsWidget::onSelectionChanged);
}

void MainToolsWidget::setDescription(QString description)
{
    ui->descriptionTextEdit->setText(description);
}

bool MainToolsWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->toolsListView->viewport() && event->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QModelIndex index       = ui->toolsListView->indexAt(mouseEvent->pos());

        if (index.isValid())
        {
            m_controller->changeSelectedTool(index);

            on_browseCheckPushButton_clicked();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MainToolsWidget::on_runAllPushButton_clicked()
{
    m_controller->runAllToolsWidget();
}

void MainToolsWidget::on_browseCheckPushButton_clicked()
{
    m_controller->chooseToolsWidget();
}

void MainToolsWidget::on_exitPushButton_clicked()
{
    m_controller->exitToolsWidget();
}

void MainToolsWidget::onSelectionChanged(const QItemSelection &newSelection, const QItemSelection &previousSelection)
{
    if (newSelection.isEmpty())
    {
        return;
    }

    QModelIndex currentIndex = newSelection.indexes().at(0);

    m_controller->changeSelectedTool(currentIndex);
}


// Метод кнопки где будет реализованно остановка теста
void MainToolsWidget::on_Stop_clicked()
{
    StopTest == true;
    if(m_controller->runAllToolsWidget() == true && StopTest == true){

        m_controller -> stopExeption();
    }else {
        return StopTest;
    }
}
