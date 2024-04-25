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
#include "../core/treemodelbulderfromexecutable.h"
#include "adtbuilderstrategies/adtmodelbuilder.h"
#include "adtbuilderstrategies/adtmodelbuilderstrategydbusinfodesktop.h"
#include "adtservicechecker.h"
#include "clcontroller.h"
#include "constants.h"
#include "interfacedata.h"
#include "interfaces/appcontrollerinterface.h"
#include "mainwindowcontrollerimpl.h"
#include "parser/commandlineoptions.h"
#include "parser/commandlineparser.h"

#include <iostream>
#include <memory>

typedef CommandLineParser::CommandLineParseResult CommandLineParseResult;

class ADTAppPrivate
{
public:
    ADTAppPrivate(QApplication *app, ADTSettingsInterface *settings, QDBusConnection conn, QString locale)
        : m_application(app)
        , m_settings(settings)
        , m_model(nullptr)
        , m_parser(new CommandLineParser(*app))
        , m_options(new CommandLineOptions())
        , m_parseErrorMessage{}
        , m_locale(locale)
        , m_appController(nullptr)
        , m_parserResult{}
        , m_serviceChecker(new ADTServiceChecker(DBUS_SERVICE_NAME, PATH_TO_MANAGER_OBJECT, MANAGER_INTERFACE_NAME))
        , m_ifaceData(new InterfaceData())
        , m_dbusConnection(conn)

    {}

    QApplication *m_application;
    ADTSettingsInterface *m_settings;
    std::unique_ptr<TreeModel> m_model;
    std::unique_ptr<CommandLineParser> m_parser;
    std::unique_ptr<CommandLineOptions> m_options;
    QString m_parseErrorMessage;
    QString m_locale;
    std::unique_ptr<AppControllerInterface> m_appController;

    CommandLineParser::CommandLineParseResult m_parserResult;

    std::unique_ptr<ADTServiceChecker> m_serviceChecker;

    std::unique_ptr<InterfaceData> m_ifaceData;

    QDBusConnection m_dbusConnection;

private:
    ADTAppPrivate(const ADTAppPrivate &) = delete;
    ADTAppPrivate(ADTAppPrivate &&)      = delete;
    ADTAppPrivate &operator=(const ADTAppPrivate &) = delete;
    ADTAppPrivate &operator=(ADTAppPrivate &&) = delete;
};

ADTApp::ADTApp(QApplication *application, ADTSettingsInterface *settings, QDBusConnection conn, QString locale)
    : d(new ADTAppPrivate(application, settings, conn, locale))
{
    initializeInterfaceData();
}

ADTApp::~ADTApp()
{
    delete d;
}

int ADTApp::runApp()
{
    d->m_parserResult = d->m_parser->parseCommandLine(d->m_options.get(), &d->m_parseErrorMessage);

    if (d->m_parserResult == CommandLineParseResult::CommandLineError)
    {
        std::cerr << d->m_parseErrorMessage.toStdString() << std::endl;
        d->m_parser->showHelp();
        return 1;
    }

    if (d->m_parserResult == CommandLineParseResult::CommandLineHelpRequested)
    {
        d->m_parser->showHelp();
        return 0;
    }

    if (d->m_parserResult == CommandLineParseResult::CommandLineVersionRequested)
    {
        d->m_parser->showVersion();
        return 0;
    }

    buildModel();

    if (d->m_options->useGraphic == true)
    {
        //use GUI
        d->m_appController = std::make_unique<MainWindowControllerImpl>(d->m_model.get(),
                                                                        d->m_dbusConnection,
                                                                        *d->m_ifaceData.get(),
                                                                        d->m_settings,
                                                                        d->m_options.get(),
                                                                        d->m_application);
    }
    else
    {
        //use CLI
        d->m_appController = std::make_unique<CLController>(d->m_model.get(),
                                                            d->m_dbusConnection,
                                                            *d->m_ifaceData.get(),
                                                            d->m_settings,
                                                            d->m_options.get());
    }

    connect(d->m_serviceChecker.get(),
            &ADTServiceChecker::serviceOwnerChanged,
            d->m_appController.get(),
            &AppControllerInterface::on_serviceOwnerChanged);
    connect(d->m_serviceChecker.get(),
            &ADTServiceChecker::serviceRegistered,
            d->m_appController.get(),
            &AppControllerInterface::on_serviceRegistered);
    connect(d->m_serviceChecker.get(),
            &ADTServiceChecker::serviceUnregistered,
            d->m_appController.get(),
            &AppControllerInterface::on_serviceUnregistered);

    return d->m_appController->runApp();
}

void ADTApp::buildModel()
{
    ADTModelBuilder modelBuilder(new ADTModelBuilderStrategyDbusInfoDesktop(d->m_ifaceData->serviceName,
                                                                            d->m_ifaceData->path,
                                                                            d->m_ifaceData->managerInterface,
                                                                            d->m_ifaceData->managerGetMethod,
                                                                            d->m_ifaceData->ifaceName,
                                                                            d->m_ifaceData->infoMethodName,
                                                                            d->m_ifaceData->runMethodName,
                                                                            d->m_ifaceData->reportMethodName,
                                                                            new TreeModelBulderFromExecutable()));
    d->m_model = std::move(modelBuilder.buildModel());
    d->m_model->setLocaleForElements(d->m_locale);
}

void ADTApp::initializeInterfaceData()
{
    d->m_ifaceData->serviceName      = DBUS_SERVICE_NAME;
    d->m_ifaceData->path             = PATH_TO_MANAGER_OBJECT;
    d->m_ifaceData->managerInterface = MANAGER_INTERFACE_NAME;
    d->m_ifaceData->managerGetMethod = MANAGER_GET_METHOD_NAME;
    d->m_ifaceData->ifaceName        = DIAG1_INTERFACE_NAME;
    d->m_ifaceData->runMethodName    = DIAG1_RUN_METHOD_NAME;
    d->m_ifaceData->infoMethodName   = DIAG1_INFO_METHOD_NAME;
    d->m_ifaceData->reportMethodName = DIAG1_REPORT_METHOD_NAME;
}
