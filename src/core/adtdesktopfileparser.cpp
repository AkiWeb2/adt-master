#include "adtdesktopfileparser.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <QDebug>

const QString ADTDesktopFileParser::NAME_KEY_NAME                = "Name";
const QString ADTDesktopFileParser::DISPLAY_NAME_KEY_NAME        = "DisplayName";
const QString ADTDesktopFileParser::ICON_KEY_NAME                = "Icon";
const QString ADTDesktopFileParser::COMMENT_KEY_NAME             = "Comment";
const QString ADTDesktopFileParser::ARGS_KEY_NAME                = "Args";
const QString ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME = "Alterator Entry";
const QString ADTDesktopFileParser::REPORT_FILE_SUFFIX_KEY_NAME  = "ReportSuffix";

ADTDesktopFileParser::ADTDesktopFileParser(QString data)
    : m_sections()
    , m_testLists()
    , m_dbusServiceName()
    , m_dbusPath()
    , m_dbusInterfaceName()
    , m_dbusInfoMethodName()
    , m_dbusRunMethodName()
    , m_dbusReportMethodName()
{
    std::istringstream iStream(data.toStdString());

    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(iStream, pt);

        for (auto &section : pt)
        {
            for (auto &key : section.second)
            {
                Section &currentSections = m_sections[QString(section.first.c_str())];

                currentSections.insert(getKeyNameWithoutLocale(QString(key.first.c_str())),
                                       IniFileKey{getKeyNameWithoutLocale(QString(key.first.c_str())),
                                                  getKeyLocale(QString(key.first.c_str())),
                                                  QVariant(key.second.get_value("").c_str())});
            }
        }
    }
    catch (std::exception &e)
    {
        qWarning() << "ERROR: can't parse desktop file: " << data << "\n";
    }
}

QString ADTDesktopFileParser::getDefaultValue(QList<IniFileKey> iniFileKey)
{
    for (IniFileKey &currentIniFileKey : iniFileKey)
    {
        if (currentIniFileKey.keyLocale.isEmpty())
        {
            return currentIniFileKey.value.toString();
        }
    }

    return QString();
}

ADTDesktopFileParser::ADTDesktopFileParser(QString data,
                                           QStringList testLists,
                                           QString dbusServiceName,
                                           QString dbusPath,
                                           QString dbusInterfaceName,
                                           QString dbusInfoMethodName,
                                           QString dbusRunMethodName,
                                           QString dbusReportMethodName)
    : ADTDesktopFileParser{data}
{
    m_testLists            = testLists;
    m_dbusServiceName      = dbusServiceName;
    m_dbusPath             = dbusPath;
    m_dbusInterfaceName    = dbusInterfaceName;
    m_dbusInfoMethodName   = dbusInfoMethodName;
    m_dbusRunMethodName    = dbusRunMethodName;
    m_dbusReportMethodName = dbusReportMethodName;
}

std::unique_ptr<ADTExecutable> ADTDesktopFileParser::buildToolExecutable()
{
    std::unique_ptr<ADTExecutable> newADTExecutable = std::make_unique<ADTExecutable>();

    QList<QString> groups = getGroupsList();

    if (groups.isEmpty())
    {
        return nullptr;
    }

    newADTExecutable->m_id = getToolName();

    if (newADTExecutable->m_id.isEmpty())
    {
        return nullptr;
    }

    newADTExecutable->m_toolId = newADTExecutable->m_id;

    QList<QString> listOfKeys = getKeysListOfGroup(ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME);

    if (listOfKeys.empty())
    {
        qWarning() << "ERROR! Can't get list of keys for object: " << newADTExecutable->m_id;

        return nullptr;
    }

    setIcon(ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME, newADTExecutable.get());

    if (!setNames(ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME, newADTExecutable.get()))
    {
        qWarning() << "ERROR! Can't get name for object: " << newADTExecutable->m_id;
        return nullptr;
    }

    if (!setDescriptions(ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME, newADTExecutable.get()))
    {
        qWarning() << "WARNING! Can't get description for object: " << newADTExecutable->m_id;
    }

    if (!setReportSuffix(newADTExecutable.get()))
    {
        qWarning() << "WARNING! Can't get report suffix for object: " << newADTExecutable->m_id;
    }

    setArgs(ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME, newADTExecutable.get());

    newADTExecutable->m_type = ADTExecutable::ExecutableType::ToolType;

    newADTExecutable->m_dbusServiceName      = m_dbusServiceName;
    newADTExecutable->m_dbusPath             = m_dbusPath;
    newADTExecutable->m_dbusInterfaceName    = m_dbusInterfaceName;
    newADTExecutable->m_dbusInfoMethodName   = m_dbusInfoMethodName;
    newADTExecutable->m_dbusRunMethodName    = m_dbusRunMethodName;
    newADTExecutable->m_dbusReportMethodName = m_dbusReportMethodName;

    return newADTExecutable;
}

std::unique_ptr<ADTExecutable> ADTDesktopFileParser::buildTestExecutable(QString test, ADTExecutable *toolExecutable)
{
    std::unique_ptr<ADTExecutable> result = std::make_unique<ADTExecutable>();

    if (!toolExecutable)
    {
        return nullptr;
    }

    result->m_id        = test.trimmed();
    result->m_type      = ADTExecutable::ExecutableType::TestType;
    result->m_name      = test.trimmed();
    result->m_toolId    = toolExecutable->m_id;
    result->m_icon      = toolExecutable->m_icon;
    result->m_exit_code = toolExecutable->m_exit_code;

    result->m_dbusServiceName   = m_dbusServiceName;
    result->m_dbusInterfaceName = m_dbusInterfaceName;
    result->m_dbusPath          = m_dbusPath;
    result->m_dbusRunMethodName = m_dbusRunMethodName;
    result->m_args              = result->m_id;

    auto testSection = m_sections.find(test);

    if (testSection == m_sections.end())
    {
        //No section for test found
        return nullptr;
    }

    if (!setNames(test, result.get()))
    {
        qWarning() << "ERROR! Can't find key " << ADTDesktopFileParser::NAME_KEY_NAME << " for test : " << test
                   << " in tool: " << toolExecutable->m_id;

        return nullptr;
    }
    if (!setDescriptions(test, result.get()))
    {
        qWarning() << "WARNING! Can't find key " << ADTDesktopFileParser::COMMENT_KEY_NAME << " for test : " << test
                   << " in tool: " << toolExecutable->m_id;
    }

    if (!setArgs(test, result.get()))
    {}

    return result;
}

std::vector<std::unique_ptr<ADTExecutable>> ADTDesktopFileParser::buildExecutables()
{
    std::vector<std::unique_ptr<ADTExecutable>> results;

    std::unique_ptr<ADTExecutable> toolExecutable = buildToolExecutable();

    if (!toolExecutable.get())
    {
        return results;
    }

    ADTExecutable *currentTool = toolExecutable.get();

    results.push_back(std::move(toolExecutable));

    for (QString currentTest : m_testLists)
    {
        std::unique_ptr<ADTExecutable> test = buildTestExecutable(currentTest, currentTool);

        if (test.get())
        {
            results.push_back(std::move(test));
        }
    }

    return results;
}

QList<QString> ADTDesktopFileParser::getGroupsList() const
{
    return m_sections.keys();
}

QList<QString> ADTDesktopFileParser::getKeysListOfGroup(QString group)
{
    return m_sections[group].keys();
}

QString ADTDesktopFileParser::getKeyLocale(QString keyName)
{
    auto indexOfOpeningBracket = keyName.lastIndexOf("[");
    auto indexOfClosingBracket = keyName.lastIndexOf("]");

    if (indexOfOpeningBracket >= indexOfClosingBracket || indexOfOpeningBracket == -1 || indexOfClosingBracket == -1)
    {
        return QString();
    }

    return keyName.mid(indexOfOpeningBracket + 1, indexOfClosingBracket - indexOfOpeningBracket - 1);
}

QString ADTDesktopFileParser::getKeyNameWithoutLocale(QString keyName)
{
    auto indexOfOpeningBracket = keyName.lastIndexOf("[");
    auto indexOfClosingBracket = keyName.lastIndexOf("]");

    if (indexOfOpeningBracket >= indexOfClosingBracket || indexOfOpeningBracket == -1 || indexOfClosingBracket == -1)
    {
        return keyName;
    }

    return keyName.mid(0, indexOfOpeningBracket);
}

bool ADTDesktopFileParser::setIcon(const QString &test, ADTExecutable *object)
{
    Section section = m_sections[test];
    auto iconIt     = section.find(ADTDesktopFileParser::ICON_KEY_NAME);

    if (iconIt == section.end())
    {
        object->m_icon = DEFAULT_ICON;
    }
    else
    {
        object->m_icon = iconIt->value.toString();
    }

    return true;
}

bool ADTDesktopFileParser::setNames(const QString &test, ADTExecutable *object)
{
    Section section = m_sections[test];

    QList<IniFileKey> listOfKeysDisplayNames = section.values(ADTDesktopFileParser::DISPLAY_NAME_KEY_NAME);

    QString defaultDisplayName = getDefaultValue(listOfKeysDisplayNames);
    if (defaultDisplayName.isEmpty())
    {
        object->m_name = object->m_id;
    }
    else
    {
        object->m_name = defaultDisplayName;
    }

    for (IniFileKey &currentIniFileKey : listOfKeysDisplayNames)
    {
        object->m_nameLocaleStorage.insert(currentIniFileKey.keyLocale, currentIniFileKey.value.toString());
    }

    return true;
}

bool ADTDesktopFileParser::setDescriptions(const QString &test, ADTExecutable *object)
{
    Section section = m_sections[test];
    auto nameIt     = section.find(ADTDesktopFileParser::COMMENT_KEY_NAME);

    if (nameIt == section.end())
    {
        object->m_description = "";
        return false;
    }

    QList<IniFileKey> listOfKeys = section.values(ADTDesktopFileParser::COMMENT_KEY_NAME);

    QString defaultDescription = getDefaultValue(listOfKeys);
    if (defaultDescription.isEmpty())
    {
        return false;
    }

    object->m_description = defaultDescription;

    for (IniFileKey &currentIniFileKey : listOfKeys)
    {
        object->m_descriptionLocaleStorage.insert(currentIniFileKey.keyLocale, currentIniFileKey.value.toString());
    }

    return true;
}

bool ADTDesktopFileParser::setArgs(const QString &test, ADTExecutable *object)
{
    Section section = m_sections[test];
    auto nameIt     = section.find(ADTDesktopFileParser::ARGS_KEY_NAME);

    if (nameIt == section.end())
    {
        object->m_args = "";
        return false;
    }

    QList<IniFileKey> listOfKeys = section.values(ADTDesktopFileParser::ARGS_KEY_NAME);

    if (!listOfKeys.empty())
    {
        object->m_args = listOfKeys.at(0).value.toString();
        return true;
    }

    return false;
}

bool ADTDesktopFileParser::setReportSuffix(ADTExecutable *object)
{
    Section section = m_sections[ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME];
    auto reportIt   = section.find(ADTDesktopFileParser::REPORT_FILE_SUFFIX_KEY_NAME);

    if (reportIt == section.end())
    {
        return false;
    }

    object->m_reportSuffix = getDefaultValue(QList<IniFileKey>{reportIt.value()});
    if (object->m_reportSuffix.isEmpty() || object->m_reportSuffix.isNull())
    {
        return false;
    }

    return true;
}

QString ADTDesktopFileParser::getToolName()
{
    auto alteratorEntryIt = m_sections.find(ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME);

    if (alteratorEntryIt == m_sections.end())
    {
        qWarning() << "ERROR! Can't find section: " << ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME
                   << " for the object located on the path " << m_dbusPath;

        return QString();
    }

    Section alteratorEntrySection = *alteratorEntryIt;

    QList<IniFileKey> toolNameList = alteratorEntrySection.values(ADTDesktopFileParser::NAME_KEY_NAME);

    if (toolNameList.isEmpty())
    {
        qWarning() << "ERROR! Can't find any keys in section: " << ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME
                   << " for the object located on the path " << m_dbusPath;

        return QString();
    }

    for (IniFileKey &currentKey : toolNameList)
    {
        if (currentKey.keyLocale.isEmpty())
        {
            return currentKey.value.toString();
        }
    }

    qWarning() << "ERROR! Can't find key: " << ADTDesktopFileParser::NAME_KEY_NAME
               << " in section: " << ADTDesktopFileParser::ALTERATOR_ENTRY_SECTION_NAME << " without locale "
               << "for the object located on the path " << m_dbusPath;
    return QString();
}
