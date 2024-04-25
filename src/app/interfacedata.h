#ifndef INTERFACEDATA_H
#define INTERFACEDATA_H

#include <QString>

struct InterfaceData
{
    QString serviceName{};
    QString path{};
    QString managerInterface{};
    QString managerGetMethod{};
    QString ifaceName{};
    QString runMethodName{};
    QString infoMethodName{};
    QString reportMethodName{};
};

#endif // INTERFACEDATA_H
