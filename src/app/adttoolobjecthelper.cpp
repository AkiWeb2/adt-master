#include "adttoolobjecthelper.h"

#include <QDBusInterface>
#include <QDBusReply>

class ADTToolObjectHelperPrivate
{
public:
    ADTToolObjectHelperPrivate(TreeItem *item)
        : m_toolItem(item)
        , m_tests(){};

public:
    TreeItem *m_toolItem = nullptr;

    QString m_filter{};

    std::vector<ADTExecutable *> m_tests;

private:
    ADTToolObjectHelperPrivate(const ADTToolObjectHelperPrivate &) = delete;
    ADTToolObjectHelperPrivate(ADTToolObjectHelperPrivate &&)      = delete;
    ADTToolObjectHelperPrivate &operator=(const ADTToolObjectHelperPrivate &) = delete;
    ADTToolObjectHelperPrivate &operator=(ADTToolObjectHelperPrivate &&) = delete;
};

ADTToolObjectHelper::ADTToolObjectHelper(TreeItem *item)
    : d(std::make_unique<ADTToolObjectHelperPrivate>(item))
{
    for (int i = 0; i < item->childCount(); i++)
    {
        TreeItem *child = item->child(i);

        d->m_tests.push_back(child->getExecutable());
    }
}

ADTToolObjectHelper::~ADTToolObjectHelper() {}

QString ADTToolObjectHelper::getId()
{
    return d->m_toolItem->getExecutable()->m_id;
}

ADTExecutable *ADTToolObjectHelper::getToolTask()
{
    return d->m_toolItem->getExecutable();
}

std::vector<ADTExecutable *> ADTToolObjectHelper::getAllTasks()
{
    return d->m_tests;
}

std::vector<ADTExecutable *> ADTToolObjectHelper::getFilteredTasks()
{
    if (d->m_filter.isEmpty())
    {
        return d->m_tests;
    }

    std::vector<ADTExecutable *> filteredTests;
    for (size_t i = 0; i < d->m_tests.size(); i++)
    {
        if (d->m_tests.at(i)->m_name.contains(d->m_filter, Qt::CaseInsensitive))
        {
            filteredTests.push_back(d->m_tests.at(i));
        }
    }
    return filteredTests;
}

ADTExecutable *ADTToolObjectHelper::getTestTask(QString test)
{
    for (size_t i = 0; i < d->m_tests.size(); i++)
    {
        if (d->m_tests.at(i)->m_id == test)
        {
            return d->m_tests.at(i);
        }
    }
    return nullptr;
}

void ADTToolObjectHelper::setFilter(QString filter)
{
    d->m_filter = filter;
}

QByteArray ADTToolObjectHelper::getReport(QDBusConnection conn)
{
    QDBusInterface iface(d->m_toolItem->getExecutable()->m_dbusServiceName,
                         d->m_toolItem->getExecutable()->m_dbusPath,
                         d->m_toolItem->getExecutable()->m_dbusInterfaceName,
                         conn);

    if (!iface.isValid())
    {
        return {};
    }

    QDBusReply<QByteArray> reply = iface.call(d->m_toolItem->getExecutable()->m_dbusReportMethodName);

    if (!reply.isValid())
    {
        return {};
    }

    return reply.value();
}
