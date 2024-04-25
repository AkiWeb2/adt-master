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

#include "categoryproxymodel.h"
#include "../core/adtexecutable.h"

CategoryProxyModel::CategoryProxyModel(const QString &category)
    : selectedCategory(category)
{}

bool CategoryProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex source_index = sourceModel()->index(source_row, this->filterKeyColumn(), source_parent);
    if (source_index.isValid())
    {
        auto data = source_index.data(Qt::UserRole + 1);
        if (data.isValid())
        {
            return data.value<ADTExecutable *>()->m_id == selectedCategory;
        }
    }

    return false;
}

void CategoryProxyModel::setSelectedCategory(const QString &category)
{
    selectedCategory = category;
}

QString CategoryProxyModel::getSelectedCategory() const
{
    return selectedCategory;
}
