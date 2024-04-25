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

#include "clickablehighlightlabel.h"
#include <QDebug>

ClickableHighlightLabel::ClickableHighlightLabel(QWidget *parent)
    : QLabel(parent)
    , m_standartColor(Qt::gray)
    , m_highlightColor(Qt::lightGray)
    , m_isDisabledDoubleClickOnLabel(false)
{
    m_highlightColor = QColor(0, 0, 0);

    m_standartColor = QColor(66, 66, 66);

    QEvent event(QEvent::Type::None);
    leaveEvent(&event);
}

void ClickableHighlightLabel::setHighlightColor(QColor color)
{
    m_highlightColor = color;
}

void ClickableHighlightLabel::disabledDoubleClick(bool disableFLag)
{
    m_isDisabledDoubleClickOnLabel = disableFLag;
}

void ClickableHighlightLabel::enterEvent(QEvent *event)
{
    QLabel::enterEvent(event);
    QPalette pal = QPalette();
    pal.setColor(QPalette::WindowText, m_highlightColor);
    setPalette(pal);
}

void ClickableHighlightLabel::leaveEvent(QEvent *event)
{
    QLabel::leaveEvent(event);
    QPalette pal = QPalette();
    pal.setColor(QPalette::WindowText, m_standartColor);
    setPalette(pal);
}

void ClickableHighlightLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    QLabel::mouseDoubleClickEvent(event);
    if (!m_isDisabledDoubleClickOnLabel)
    {
        emit this->doubleClicked();
    }
}
