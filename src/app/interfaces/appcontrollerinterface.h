#ifndef APPCONTROLLERINTERFACE_H
#define APPCONTROLLERINTERFACE_H

#include <QObject>
#include <QString>

#include "../core/adtexecutable.h"

class AppControllerInterface : public QObject
{
public:
    Q_OBJECT
public:
    virtual ~AppControllerInterface();

    virtual int listObjects() = 0;

    virtual int listTestsOfObject(QString object) = 0;

    virtual int runAllTestsOfObject(QString object) = 0;

    virtual int runSpecifiedTestOfObject(QString object, QString test) = 0;

    virtual int getToolReport(QString tool, QString file) = 0;

    virtual int runApp() = 0;

public slots:
    virtual void on_serviceUnregistered() = 0;
    virtual void on_serviceRegistered()   = 0;
    virtual void on_serviceOwnerChanged() = 0;

protected:
    virtual void onAllTasksBegin()    = 0;
    virtual void onAllTasksFinished() = 0;

    virtual void onBeginTask(ADTExecutable *task)  = 0;
    virtual void onFinishTask(ADTExecutable *task) = 0;
};

#endif // APPCONTROLLERINTERFACE_H
