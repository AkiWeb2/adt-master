#include "treemodelbulderfromexecutable.h"

std::unique_ptr<TreeModel> TreeModelBulderFromExecutable::buildModel(std::vector<std::unique_ptr<ADTExecutable>> elements)
{
    std::unique_ptr<TreeModel> model = std::make_unique<TreeModel>();

    QModelIndex rootIndex = model->parent(QModelIndex());

    TreeItem *rootItem = static_cast<TreeItem *>(rootIndex.internalPointer());

    QMap<QString, TreeItem *> categoriesMap;

    for (size_t i = 0; i < elements.size(); i++)
    {
        if (elements.at(i)->m_type == ADTExecutable::ExecutableType::ToolType)
        {
            TreeItem *categoryItem = new TreeItem(QList<QVariant>{}, TreeItem::categoryItem, rootItem);

            rootItem->appendChild(categoryItem);

            categoriesMap[elements.at(i)->m_id] = categoryItem;

            categoryItem->setIcon(elements.at(i)->m_icon);
            categoryItem->setExecutable(std::move(elements.at(i)));
        }
    }
    for (size_t i = 0; i < elements.size(); i++)
    {
        if (elements.at(i) != NULL && elements.at(i)->m_type == ADTExecutable::ExecutableType::TestType)
        {
            auto it = categoriesMap.find(elements.at(i)->m_toolId);

            if (it == categoriesMap.end())
            {
                qWarning() << "ERROR! Can't find category: " << elements.at(i)->m_id
                           << " for element: " << elements.at(i)->m_id;

                continue;
            }

            TreeItem *categoryItem = categoriesMap[elements.at(i)->m_toolId];

            TreeItem *checkItem = new TreeItem(QList<QVariant>{}, TreeItem::checkItem, categoryItem);

            categoryItem->appendChild(checkItem);

            checkItem->setIcon(elements.at(i)->m_icon);
            checkItem->setExecutable(std::move(elements.at(i)));
        }
    }

    return model;
}
