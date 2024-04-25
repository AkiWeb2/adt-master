#ifndef MAINWINDOWCONTROLLERINTERFACE_H
#define MAINWINDOWCONTROLLERINTERFACE_H

#include "../core/adtexecutable.h"
#include "../core/treeitem.h"

#include <mainwindow/statuscommonwidget.h>

class MainWindowControllerInterface
{
public:
    virtual ~MainWindowControllerInterface();

    // Run All button pressed in tools widget
    virtual void runAllToolsWidget() = 0;

    //Browse button pressed in tools widget
    virtual void chooseToolsWidget() = 0;

    //Exit button pressed in tools widget
    virtual void exitToolsWidget() = 0;

    //Changes current selected tool
    virtual void changeSelectedTool(QModelIndex index) = 0;

    //Return Model Item of slected tool
    virtual TreeItem *getTreeItemByIndex(QModelIndex index) = 0;

    //Switch to tests widget and runs task in test vector
    virtual void runTestsWidget(std::vector<ADTExecutable *> tests) = 0;

    //Switch to test widget and runs all filtered tests
    virtual void runCurrentToolTest() = 0;

    //Back button pressed in tests widget
    virtual void backTestsWigdet() = 0;

    //Exit button pressed in tests widget
    virtual void exitTestsWidget() = 0;

    //Report button pressed in testswidget
    virtual void reportTestWidget() = 0;

    //Details button pressed in tests widget
    virtual void detailsCurrentTest(StatusCommonWidget *widget) = 0;

    //Runs apps with GUI or CLI
    virtual int runApp() = 0;

    //Saves mainwindow settings in common settings object
    virtual void saveMainWindowSettings() = 0;

    //Restores mainwindow settings form common settings object
    virtual void restoreMainWindowSettings() = 0;
};

#endif
