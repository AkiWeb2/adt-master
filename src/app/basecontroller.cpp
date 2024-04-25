#include "basecontroller.h"

void BaseController::buildToolHelpers(TreeModel *model, std::vector<std::unique_ptr<ADTToolObjectHelper>> &helpers)
{
    std::vector<std::unique_ptr<ADTToolObjectHelper>> newHelpers;

    QModelIndex rootIndex = model->parent(QModelIndex());
    TreeItem *rootItem    = static_cast<TreeItem *>(rootIndex.internalPointer());

    int numTools = rootItem->childCount();

    for (int i = 0; i < numTools; ++i)
    {
        TreeItem *tool                                 = rootItem->child(i);
        std::unique_ptr<ADTToolObjectHelper> newHelper = std::make_unique<ADTToolObjectHelper>(tool);
        newHelpers.push_back(std::move(newHelper));
    }

    std::for_each(newHelpers.begin(), newHelpers.end(), [&helpers](std::unique_ptr<ADTToolObjectHelper> &currentHelper) {
        helpers.push_back(std::move(currentHelper));
    });
}

int BaseController::listObjects()
{
    return 0;
}

int BaseController::listTestsOfObject(QString object)
{
    return 0;
}

int BaseController::runAllTestsOfObject(QString object)
{
    return 0;
}

int BaseController::runSpecifiedTestOfObject(QString object, QString test)
{
    return 0;
}

int BaseController::getToolReport(QString tool, QString file)
{
    return 0;
}

int BaseController::runApp()
{
    return 0;
}

void BaseController::on_serviceUnregistered() {}

void BaseController::on_serviceRegistered() {}

void BaseController::on_serviceOwnerChanged() {}

void BaseController::onAllTasksBegin() {}

void BaseController::onAllTasksFinished() {}

void BaseController::onBeginTask(ADTExecutable *task) {}

void BaseController::onFinishTask(ADTExecutable *task) {}
