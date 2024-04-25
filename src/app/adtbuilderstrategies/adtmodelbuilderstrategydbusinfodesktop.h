#ifndef ADTMODELBUILDERSTRATEGYDBUSINFODESKTOP_H
#define ADTMODELBUILDERSTRATEGYDBUSINFODESKTOP_H

#include "../core/treemodelbuilderinterface.h"
#include "adtmodelbuilderstrategyinterface.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QString>

class ADTModelBuilderStrategyDbusInfoDesktop : public ADTModelBuilderStrategyInterface
{
public:
    static const QString LIST_METHOD;
    static const QString INFO_METHOD;

    ADTModelBuilderStrategyDbusInfoDesktop(QString serviceName,
                                           QString path,
                                           QString interface,
                                           QString getMethodName,
                                           QString findInterface,
                                           QString infoMethodName,
                                           QString runTaskMethodName,
                                           QString reportMethodName,
                                           TreeModelBuilderInterface *builder);

public:
    std::unique_ptr<TreeModel> buildModel() override;

private:
    QStringList getObjectsPathByInterface(QString interface);

    std::vector<std::unique_ptr<ADTExecutable>> buildADTExecutablesFromDesktopFile(QString path);

private:
    QString m_serviceName;
    QString m_path;
    QString m_interface;
    QString m_get_method_name;
    QString m_findInterface;
    QString m_infoMethodName;
    QString m_runMethodName;
    QString m_reportMethodName;

    std::unique_ptr<TreeModelBuilderInterface> m_treeModelBuilder;

    QList<QString> m_implementedInterfacesPath;

    std::unique_ptr<QDBusConnection> m_dbus;
    std::unique_ptr<QDBusInterface> m_dbusInterface;
};

#endif // ADTMODELBUILDERSTRATEGYDBUSINFODESKTOP_H
