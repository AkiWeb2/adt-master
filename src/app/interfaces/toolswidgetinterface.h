#ifndef TOOLSWIDGETINTERFACE_H
#define TOOLSWIDGETINTERFACE_H

#include <QAbstractItemModel>
#include <QString>

#include "mainwindowcontrollerinterface.h"

class ToolsWidgetInterface
{
public:
    virtual ~ToolsWidgetInterface();

    virtual void setModel(QAbstractItemModel *model) = 0;

    virtual void enableButtons()  = 0;
    virtual void disableButtons() = 0;

    virtual void enableExitButton()  = 0;
    virtual void disableExitButton() = 0;

    virtual void setDescription(QString description) = 0;

    virtual void setController(MainWindowControllerInterface *controller) = 0;
};

#endif
