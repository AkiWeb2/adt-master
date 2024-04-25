#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include "../core/treemodel.h"
#include "adttoolobjecthelper.h"
#include "interfaces/appcontrollerinterface.h"
#include <memory>
#include <vector>

class BaseController : public AppControllerInterface
{
public:
    BaseController()  = default;
    ~BaseController() = default;

public:
    void buildToolHelpers(TreeModel *model, std::vector<std::unique_ptr<ADTToolObjectHelper>> &vec);

public:
    int listObjects() override;
    int listTestsOfObject(QString object) override;
    int runAllTestsOfObject(QString object) override;
    int runSpecifiedTestOfObject(QString object, QString test) override;
    int getToolReport(QString tool, QString file) override;
    int runApp() override;

public slots:
    void on_serviceUnregistered() override;
    void on_serviceRegistered() override;
    void on_serviceOwnerChanged() override;

protected:
    void onAllTasksBegin() override;
    void onAllTasksFinished() override;
    void onBeginTask(ADTExecutable *task) override;
    void onFinishTask(ADTExecutable *task) override;
};

#endif // BASECONTROLLER_H
