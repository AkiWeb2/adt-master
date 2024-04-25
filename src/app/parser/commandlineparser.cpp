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

#include "parser/commandlineparser.h"

#include <memory>
#include <QApplication>
#include <QCommandLineParser>

typedef CommandLineParser::CommandLineParseResult CommandLineParseResult;

class CommandLineParserPrivate
{
public:
    QApplication &application;
    std::unique_ptr<QCommandLineParser> parser;

    CommandLineParserPrivate(QApplication &currentApplication)
        : application(currentApplication)
        , parser(std::make_unique<QCommandLineParser>())
    {}

private:
    CommandLineParserPrivate(const CommandLineParserPrivate &) = delete;
    CommandLineParserPrivate(CommandLineParserPrivate &&)      = delete;
    CommandLineParserPrivate &operator=(const CommandLineParserPrivate &) = delete;
    CommandLineParserPrivate &operator=(CommandLineParserPrivate &&) = delete;
};

CommandLineParser::CommandLineParser(QApplication &application)
    : d(new CommandLineParserPrivate(application))
{}

CommandLineParser::~CommandLineParser()
{
    delete d;
}

CommandLineParseResult CommandLineParser::parseCommandLine(CommandLineOptions *options, QString *errorMessage)
{
    const QCommandLineOption helpOption = d->parser->addHelpOption();

    const QCommandLineOption versionOption = d->parser->addVersionOption();

    const QCommandLineOption listOfObjectsOption(QStringList() << "l"
                                                               << "list",
                                                 QObject::tr("List of available tools"));

    const QCommandLineOption objectListOption(QStringList() << "o"
                                                            << "object",
                                              QObject::tr("Test list of specified tool."),
                                              "tool");

    const QCommandLineOption
        runSpecifiedTestOption(QStringList() << "r"
                                             << "run",
                               QObject::tr(
                                   "Runs the tests for the specified instrument. If no specific test is specified with "
                                   "the -t option, all tests for the specified instrument are run."),
                               "tool");

    const QCommandLineOption specifiedTestOption(QStringList() << "t"
                                                               << "test",
                                                 QObject::tr("Specify test for running"),
                                                 "test");

    const QCommandLineOption useGraphicOption(QStringList() << "g"
                                                            << "graph",
                                              QObject::tr("if specified, the graphical user interface will be used. If "
                                                          "not specified, the command line interface will be used "));

    const QCommandLineOption toolReportOption(QStringList() << "R"
                                                            << "Report",
                                              QObject::tr("Get report from tool. Tool must be specified."),
                                              "tool");

    const QCommandLineOption reportFilePath(QStringList() << "f"
                                                          << "file",
                                            QObject::tr("Specifies to which file to save the report."),
                                            "file");

    d->parser->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    d->parser->addOption(objectListOption);
    d->parser->addOption(listOfObjectsOption);
    d->parser->addOption(runSpecifiedTestOption);
    d->parser->addOption(specifiedTestOption);
    d->parser->addOption(useGraphicOption);
    d->parser->addOption(toolReportOption);
    d->parser->addOption(reportFilePath);

    if (!d->parser->parse(d->application.arguments()))
    {
        *errorMessage = d->parser->errorText();
        return CommandLineError;
    }

    options->useGraphic = false;

    if (d->parser->isSet(versionOption))
    {
        return CommandLineVersionRequested;
    }

    if (d->parser->isSet(helpOption))
    {
        return CommandLineHelpRequested;
    }

    if (d->parser->isSet(listOfObjectsOption))
    {
        if (d->parser->isSet(useGraphicOption))
        {
            options->useGraphic = true;
        }

        return CommandLineListOfObjectsRequested;
    }

    if (d->parser->isSet(objectListOption))
    {
        const QString name = d->parser->value(objectListOption);

        if (name.isNull() || name.isEmpty())
        {
            *errorMessage = QObject::tr("Bad object name: ") + name;
            return CommandLineError;
        }

        QString objectName = name.mid(name.lastIndexOf("/") + 1, name.size());

        options->objectName = objectName;

        options->action = CommandLineOptions::Action::listOfTestFromSpecifiedObject;

        if (d->parser->isSet(useGraphicOption))
        {
            options->useGraphic = true;
        }

        return CommandLineListOfTestsRequested;
    }

    if (d->parser->isSet(runSpecifiedTestOption))
    {
        const QString objectName = d->parser->value(runSpecifiedTestOption);
        options->objectName      = objectName;

        if (options->objectName.isNull() || options->objectName.isEmpty())
        {
            *errorMessage = QObject::tr("Bad object name: ") + objectName;
            return CommandLineError;
        }

        if (d->parser->isSet(useGraphicOption))
        {
            options->useGraphic = true;
        }

        if (d->parser->isSet(specifiedTestOption))
        {
            const QString testName = d->parser->value(specifiedTestOption);
            options->testName      = testName;
            if (options->testName.isNull() || options->testName.isEmpty())
            {
                *errorMessage = QObject::tr("Bad test name: ") + testName;
                return CommandLineError;
            }

            options->action = CommandLineOptions::Action::runSpecifiedTestFromSpecifiedObject;
            return CommandLineRunSpecifiedTestRequested;
        }
        else
        {
            options->action = CommandLineOptions::Action::runAllTestFromSpecifiedObject;
            return CommandLineRunAllTestsRequested;
        }
    }

    if (d->parser->isSet(toolReportOption))
    {
        const QString tool = d->parser->value(toolReportOption);
        if (tool.isEmpty())
        {
            *errorMessage = QObject::tr("Tool name must be specified.");
            return CommandLineError;
        }

        if (!d->parser->isSet(reportFilePath))
        {
            *errorMessage = QObject::tr("The file to save the report must be specified.");
            return CommandLineError;
        }

        const QString file = d->parser->value(reportFilePath);

        if (file.isEmpty())
        {
            *errorMessage = QObject::tr("Wrong file to save the report specified.");
            return CommandLineError;
        }

        options->action         = CommandLineOptions::Action::getReportTool;
        options->objectName     = tool;
        options->reportFilename = file;

        if (d->parser->isSet(useGraphicOption))
        {
            options->useGraphic = true;
        }

        return CommandLineGetReportSpecifiedTool;
    }
    options->useGraphic = true;

    return CommandLineOk;
}

void CommandLineParser::showHelp()
{
    d->parser->showHelp();
}

void CommandLineParser::showVersion()
{
    d->parser->showVersion();
}
