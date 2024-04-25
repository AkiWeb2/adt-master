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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "interfaces/mainwindowinterface.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindowPrivate;

class MainWindow : public QMainWindow, public MainWindowInterface
{
public:
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeAll() override;

    void setController(MainWindowControllerInterface *controller);

    void toggleStackWidget() override;

    ToolsWidgetInterface *getToolsWidget() override;
    TestWidgetInterface *getTestWidget() override;

private:
    void closeEvent(QCloseEvent *closeEvent) override;

private:
    Ui::MainWindow *ui;

    MainWindowPrivate *d;

private:
    MainWindow(const MainWindow &) = delete;
    MainWindow(MainWindow &&)      = delete;
    MainWindow &operator=(const MainWindow &) = delete;
    MainWindow &operator=(MainWindow &&) = delete;
};

#endif // MAINWINDOW_H
