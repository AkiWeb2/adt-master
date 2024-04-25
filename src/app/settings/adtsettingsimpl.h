/***********************************************************************************************************************
**
** Copyright (C) 2024 BaseALT Ltd. <org@basealt.ru>
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
#ifndef ADTSETTINGSIMPL_H
#define ADTSETTINGSIMPL_H

#include "adtsettingsinterface.h"

class ADTSettingsPrivate;

class ADTSettingsImpl : public ADTSettingsInterface
{
public:
    ADTSettingsImpl();
    ~ADTSettingsImpl();

public:
    void restoreWindowSettings(QWidget *window) override;
    void saveWindowsSettings(QWidget *window) override;

    void saveReportPath(QString path) override;
    QString getReportPath() override;

    void saveReportFilenameTemplate(QString templ) override;
    QString getReportFilenameTemplate() override;

private:
    std::unique_ptr<ADTSettingsPrivate> d;

private:
    ADTSettingsImpl(const ADTSettingsImpl &) = delete;
    ADTSettingsImpl(ADTSettingsImpl &&)      = delete;
    ADTSettingsImpl &operator=(const ADTSettingsImpl &) = delete;
    ADTSettingsImpl &operator=(ADTSettingsImpl &&) = delete;
};

#endif //ADTSETTINGSIMPL_H
