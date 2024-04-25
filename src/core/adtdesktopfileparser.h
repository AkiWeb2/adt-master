#ifndef ADTDESKTOPFILEPARSER_H
#define ADTDESKTOPFILEPARSER_H

#include "adtexecutable.h"

#include <unordered_map>
#include <QString>
#include <QVariant>

struct IniFileKey
{
    QString keyName{};
    QString keyLocale{};
    QVariant value{};
};

class ADTDesktopFileParser
{
public:
    using Section  = QMultiMap<QString, IniFileKey>;
    using Sections = QMap<QString, Section>;

    const QString DEFAULT_ICON = "system-run";

    static const QString ALTERATOR_ENTRY_SECTION_NAME;
    static const QString NAME_KEY_NAME;
    static const QString DISPLAY_NAME_KEY_NAME;
    static const QString ICON_KEY_NAME;
    static const QString REPORT_FILE_SUFFIX_KEY_NAME;
    static const QString COMMENT_KEY_NAME;
    static const QString ARGS_KEY_NAME;

public:
    ADTDesktopFileParser(QString data,
                         QStringList testLists,
                         QString dbusServiceName,
                         QString dbusPath,
                         QString dbusInterfaceName,
                         QString dbusInfoMethodName,
                         QString dbusRunMethodName,
                         QString dbusReportMethodName);

    QList<QString> getGroupsList() const;
    QList<QString> getKeysListOfGroup(QString group);

    QString getKeyLocale(QString keyName);
    std::vector<std::unique_ptr<ADTExecutable>> buildExecutables();

private:
    ADTDesktopFileParser(QString data);

    std::unique_ptr<ADTExecutable> buildToolExecutable();
    std::unique_ptr<ADTExecutable> buildTestExecutable(QString test, ADTExecutable *toolExecutable);

    QString getDefaultValue(QList<IniFileKey> iniFileKey);

private:
    QString getKeyNameWithoutLocale(QString keyName);
    bool setIcon(const QString &test, ADTExecutable *object);
    bool setNames(const QString &test, ADTExecutable *object);
    bool setDescriptions(const QString &test, ADTExecutable *object);
    bool setArgs(const QString &test, ADTExecutable *object);
    bool setReportSuffix(ADTExecutable *object);
    QString getToolName();

private:
    Sections m_sections;

    QStringList m_testLists;

    QString m_dbusServiceName;
    QString m_dbusPath;
    QString m_dbusInterfaceName;
    QString m_dbusInfoMethodName;
    QString m_dbusRunMethodName;
    QString m_dbusReportMethodName;
};

#endif // ADTDESKTOPFILEPARSER_H
