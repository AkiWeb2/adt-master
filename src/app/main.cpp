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

#include "adtapp.h"
#include "settings/adtsettingsimpl.h"

#include <iostream>
#include <QApplication>
#include <QDBusConnection>
#include <QDebug>
#include <QTranslator>
#include <QIcon>

#include "version.h"

QString getVersion()
{
    return QString("%1.%2.%3").arg(0).arg(ADT_VERSION_MINOR).arg(ADT_VERSION_PATCH);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // NOTE: set app variables which will be used to
    // construct settings path
    app.setOrganizationName(QCoreApplication::translate("main", "BaseALT"));
    app.setOrganizationDomain("basealt.ru");
    app.setApplicationName("ADT");
    app.setApplicationVersion(getVersion());
    app.setWindowIcon(QIcon(":adt.ico"));

    QLocale locale;

    QTranslator translator;
    QString language = locale.system().name().split("_").at(0);
    translator.load("app_" + language, ":/");
    app.installTranslator(&translator);

    ADTSettingsImpl settings;

    QDBusConnection connection(QDBusConnection::systemBus());

    ADTApp adtApp(&app, &settings, connection, locale.system().name());

    return adtApp.runApp();
}
