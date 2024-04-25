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

#include "statuscommonwidget.h"
#include "clickablehighlightlabel.h"
#include "ui_statuscommonwidget.h"

#include <QStyle>

StatusCommonWidget::StatusCommonWidget(ADTExecutable *exec, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatusCommonWidget)
    , executable(exec)
    , detailsDialog(new DetailsDialog())
    , m_defaultColor()

{
    ui->setupUi(this);

    QIcon icon = style()->standardIcon(QStyle::SP_ComputerIcon);

    ui->testIconLabel->setPixmap(icon.pixmap(16, 16));

    ui->testNameLabel->setText(executable->m_name.trimmed());

    m_defaultColor = QWidget::palette().color(QWidget::backgroundRole());

    connect(ui->testNameLabel,
            &ClickableHighlightLabel::doubleClicked,
            this,
            &StatusCommonWidget::on_runPushButton_clicked);

    connect(executable, &ADTExecutable::getStdoutLine, detailsDialog, &DetailsDialog::on_getStdout);
    connect(executable, &ADTExecutable::getStderrLine, detailsDialog, &DetailsDialog::on_getStderr);
}

StatusCommonWidget::~StatusCommonWidget()
{
    delete detailsDialog;
    delete ui;
}

ADTExecutable *StatusCommonWidget::getExecutable()
{
    return executable;
}

void StatusCommonWidget::setText(QString text)
{
    ui->testNameLabel->setText(text);
}

void StatusCommonWidget::setIcon(QIcon &icon)
{
    ui->testIconLabel->setPixmap(icon.pixmap(QSize(16, 16)));
}

void StatusCommonWidget::setEnabledRunButton(bool isEnabled)
{
    ui->runPushButton->setEnabled(isEnabled);
}

void StatusCommonWidget::setWidgetStatus(StatusCommonWidget::WidgetStatus status)
{
    QIcon icon;
    WidgetBackgroundColor backColor;
    QString text;
    QPalette pal = QPalette();

    switch (status)
    {
    case WidgetStatus::ready:
        icon      = style()->standardIcon(QStyle::SP_ComputerIcon);
        text      = executable->m_name;
        backColor = TestReadyColor();
        break;
    case WidgetStatus::running:
        icon      = style()->standardIcon(QStyle::SP_BrowserReload);
        text      = QString(tr("Running:")) + QString(" ") + executable->m_name;
        backColor = TestRunningColor();
        break;
    case WidgetStatus::finishedOk:
        icon      = style()->standardIcon(QStyle::SP_DialogApplyButton);
        text      = executable->m_name;
        backColor = TestOkColor();
        break;
    case WidgetStatus::finishedFailed:
        icon      = style()->standardIcon(QStyle::SP_DialogCloseButton);
        text      = executable->m_name;
        backColor = TestFailedColor();
        break;
    }

    QColor color(backColor.red, backColor.green, backColor.blue);
    pal.setColor(QPalette::Window, color);
    setPalette(pal);
    setIcon(icon);
    setText(text.trimmed());
    setAutoFillBackground(true);
}

void StatusCommonWidget::disableRunTestByDoubleClick(bool flag)
{
    ui->testNameLabel->disabledDoubleClick(flag);
}

DetailsDialog *StatusCommonWidget::getDetailsDialog()
{
    return detailsDialog;
}

void StatusCommonWidget::on_runPushButton_clicked()
{
    emit runButtonCLicked(this);
}

void StatusCommonWidget::on_logsPushButton_clicked()
{
    emit logsButtonClicked(this);
}
