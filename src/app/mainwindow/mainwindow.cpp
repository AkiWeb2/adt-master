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

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>

class MainWindowPrivate
{
public:
    MainWindowPrivate(MainWindow *mWindow, Ui::MainWindow *ui)
        : mainWindow(mWindow)
        , mainWindowUi(ui)
        , controller(nullptr)
    {}
    ~MainWindowPrivate() = default;

    MainWindow *mainWindow;

    Ui::MainWindow *mainWindowUi;

    MainWindowControllerInterface *controller;

private:
    MainWindowPrivate(const MainWindowPrivate &) = delete;
    MainWindowPrivate(MainWindowPrivate &&)      = delete;
    MainWindowPrivate &operator=(const MainWindowPrivate &) = delete;
    MainWindowPrivate &operator=(MainWindowPrivate &&) = delete;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow())
    , d(nullptr)
{
    ui->setupUi(this);
    d = new MainWindowPrivate(this, ui);
}

MainWindow::~MainWindow()
{
    delete d;
    delete ui;
}

void MainWindow::closeAll()
{
    d->controller->saveMainWindowSettings();
    close();
}

void MainWindow::setController(MainWindowControllerInterface *controller)
{
    d->controller = controller;
    d->controller->restoreMainWindowSettings();
}

void MainWindow::toggleStackWidget()
{
    ui->stackedWidget->currentIndex() == 0 ? ui->stackedWidget->setCurrentIndex(1)
                                           : ui->stackedWidget->setCurrentIndex(0);
}

ToolsWidgetInterface *MainWindow::getToolsWidget()
{
    return ui->toolsPage;
}

TestWidgetInterface *MainWindow::getTestWidget()
{
    return ui->testsPage;
}

void MainWindow::closeEvent(QCloseEvent *closeEvent)
{
    d->controller->saveMainWindowSettings();

    QWidget *currentWidget = ui->stackedWidget->currentWidget();
    if (currentWidget == ui->toolsPage)
    {
        d->controller->exitToolsWidget();
    }
    else
    {
        d->controller->exitTestsWidget();
    }
}
