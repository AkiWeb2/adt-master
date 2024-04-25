#ifndef ADTTOOLOBJECTHELPER_H
#define ADTTOOLOBJECTHELPER_H

#include <memory.h>
#include <vector>
#include <QDBusConnection>

#include "../core/treeitem.h"

class ADTToolObjectHelperPrivate;

class ADTToolObjectHelper : public QObject
{
public:
    Q_OBJECT
public:
    ADTToolObjectHelper(TreeItem *item);
    ~ADTToolObjectHelper();

    QString getId();
    ADTExecutable *getToolTask();

    std::vector<ADTExecutable *> getAllTasks();
    std::vector<ADTExecutable *> getFilteredTasks();
    ADTExecutable *getTestTask(QString test);

    void setFilter(QString filter);

    QByteArray getReport(QDBusConnection conn);

private:
    std::unique_ptr<ADTToolObjectHelperPrivate> d;

private:
    ADTToolObjectHelper(const ADTToolObjectHelper &) = delete;
    ADTToolObjectHelper(ADTToolObjectHelper &&)      = delete;
    ADTToolObjectHelper &operator=(const ADTToolObjectHelper) = delete;
    ADTToolObjectHelper &operator=(ADTToolObjectHelper &&) = delete;
};

#endif // ADTTOOLOBJECTHELPER_H
