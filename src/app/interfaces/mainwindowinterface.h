#ifndef MAINWINDOWINTERFACE_H
#define MAINWINDOWINTERFACE_H

#include "mainwindowcontrollerinterface.h"
#include "testswidgetinterface.h"
#include "toolswidgetinterface.h"

class MainWindowInterface
{
public:
    virtual ~MainWindowInterface();

    virtual void setController(MainWindowControllerInterface *controller) = 0;

    virtual void closeAll() = 0;

    virtual void toggleStackWidget() = 0;

    virtual ToolsWidgetInterface *getToolsWidget() = 0;

    virtual TestWidgetInterface *getTestWidget() = 0;
};

#endif
