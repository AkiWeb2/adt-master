/***********************************************************************************************************************
**
** Copyright (C) 2022 BaseALT Ltd. <org@basealt.ru>
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

#include "adtjsonconverter.h"

#include <QMetaProperty>
#include <QObject>

QJsonObject ADTJsonConverter::propertyToJson(const QObject &obj)
{
    QJsonObject json;

    for (int i = 1; i < obj.metaObject()->propertyCount(); i++)
    {
        QMetaProperty prop = obj.metaObject()->property(i);

        auto propName = prop.name();

        json[propName] = obj.property(propName).toString();
    }

    return json;
}

void ADTJsonConverter::JSonToObject(QObject &obj, QJsonObject *json)
{
    QVariantMap map = json->toVariantMap();

    for (auto key : map.keys())
    {
        obj.setProperty(key.toStdString().c_str(), map.value(key));
    }
}
