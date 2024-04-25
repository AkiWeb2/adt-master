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

#include "adtmodelbuilderstrategydbusinfodesktop.h"
#include "../core/adtdesktopfileparser.h"

#include <QDBusReply>
#include <QDebug>
#include <QJsonDocument>

const QString ADTModelBuilderStrategyDbusInfoDesktop::LIST_METHOD = QString("List");
const QString ADTModelBuilderStrategyDbusInfoDesktop::INFO_METHOD = QString("Info");

ADTModelBuilderStrategyDbusInfoDesktop::ADTModelBuilderStrategyDbusInfoDesktop(QString serviceName,
                                                                               QString path,
                                                                               QString interface,
                                                                               QString getMethodName,
                                                                               QString findInterface,
                                                                               QString infoMethodName,
                                                                               QString runTaskMethodName,
                                                                               QString reportMethodName,
                                                                               TreeModelBuilderInterface *builder)
    : m_serviceName(serviceName)
    , m_path(path)
    , m_interface(interface)
    , m_get_method_name(getMethodName)
    , m_findInterface(findInterface)
    , m_infoMethodName(infoMethodName)
    , m_runMethodName(runTaskMethodName)
    , m_reportMethodName(reportMethodName)
    , m_treeModelBuilder(builder)
    , m_implementedInterfacesPath()
    , m_dbus(new QDBusConnection(QDBusConnection::systemBus()))
    , m_dbusInterface(new QDBusInterface(m_serviceName, m_path, m_interface, *m_dbus.get()))
{}

std::unique_ptr<TreeModel> ADTModelBuilderStrategyDbusInfoDesktop::buildModel()
{
    QStringList listOfObjects = getObjectsPathByInterface(m_findInterface);

    if (listOfObjects.isEmpty())
    {
        qWarning() << "ERROR! Can't get list of tools!";

        return std::unique_ptr<TreeModel>(new TreeModel());
    }

    std::vector<std::unique_ptr<ADTExecutable>> adtExecutables;

    for (QString currentPath : listOfObjects)
    {
        std::vector<std::unique_ptr<ADTExecutable>> currentExecutables = buildADTExecutablesFromDesktopFile(currentPath);
        if (!currentExecutables.empty())
        {
            for (auto &currentExe : currentExecutables)
            {
                adtExecutables.push_back(std::move(currentExe));
            }
        }
    }

    return m_treeModelBuilder->buildModel(std::move(adtExecutables));
}

QStringList ADTModelBuilderStrategyDbusInfoDesktop::getObjectsPathByInterface(QString interface)
{
    QDBusReply<QList<QDBusObjectPath>> reply = m_dbusInterface->call(m_get_method_name, interface);

    QList<QDBusObjectPath> pathList = reply.value();

    QStringList paths;

    std::for_each(pathList.begin(), pathList.end(), [&paths](QDBusObjectPath &path) { paths.append(path.path()); });

    return paths;
}

std::vector<std::unique_ptr<ADTExecutable>> ADTModelBuilderStrategyDbusInfoDesktop::buildADTExecutablesFromDesktopFile(
    QString path)
{
    QDBusInterface iface(m_serviceName, path, m_findInterface, *m_dbus.get());

    QDBusReply<QStringList> testsListReply = iface.call(ADTModelBuilderStrategyDbusInfoDesktop::LIST_METHOD);

    if (!testsListReply.isValid())
    {
        qWarning() << "ERROR! Can't answer from list method from object with path: " << path;

        return std::vector<std::unique_ptr<ADTExecutable>>();
    }

    QStringList testsList = testsListReply.value();

    for (QString &currentTestName : testsList)
    {
        currentTestName = currentTestName.trimmed();
    }

    if (testsList.isEmpty())
    {
        qWarning() << "ERROR! Can't get list of tests from object with path: " << path;

        return std::vector<std::unique_ptr<ADTExecutable>>();
    }

    QDBusReply<QByteArray> reply = iface.call(ADTModelBuilderStrategyDbusInfoDesktop::INFO_METHOD);

    if (!reply.isValid())
    {
        qWarning() << "ERROR! Can't answer from info method from object with path: " << path;

        return std::vector<std::unique_ptr<ADTExecutable>>();
    }

    if (reply.value().isEmpty())
    {
        qWarning() << "ERROR! Can't get info from object with path: " << path;

        return std::vector<std::unique_ptr<ADTExecutable>>();
    }

    QString info(reply.value());

    ADTDesktopFileParser parser(info,
                                testsList,
                                m_serviceName,
                                path,
                                m_findInterface,
                                m_infoMethodName,
                                m_runMethodName,
                                m_reportMethodName);

    return parser.buildExecutables();
}
