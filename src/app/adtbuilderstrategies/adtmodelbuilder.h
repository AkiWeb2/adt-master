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

#ifndef ADTMODELBUILDER_H
#define ADTMODELBUILDER_H

#include "adtbuilderstrategies/adtmodelbuilderstrategyinterface.h"

#include <memory>

class ADTModelBuilder
{
public:
    ADTModelBuilder(ADTModelBuilderStrategyInterface *builderStrategy);

    std::unique_ptr<TreeModel> buildModel();

private:
    std::unique_ptr<ADTModelBuilderStrategyInterface> m_builder;
};

#endif // ADTMODELBUILDER_H
