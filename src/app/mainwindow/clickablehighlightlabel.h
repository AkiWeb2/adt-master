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

#ifndef CLICKABLEHIGHLIGHTLABEL_H
#define CLICKABLEHIGHLIGHTLABEL_H

#include <QEvent>
#include <QLabel>

class ClickableHighlightLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableHighlightLabel(QWidget *parent = nullptr);

    void setHighlightColor(QColor color);

    void disabledDoubleClick(bool disableFLag);

signals:
    void clicked();
    void doubleClicked();

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QColor m_standartColor;
    QColor m_highlightColor;
    bool m_isDisabledDoubleClickOnLabel;
};

#endif // CLICKABLEHIGHLIGHTLABEL_H
