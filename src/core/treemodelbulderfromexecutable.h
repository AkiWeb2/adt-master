#ifndef TREEMODELBULDERFROMEXECUTABLE_H
#define TREEMODELBULDERFROMEXECUTABLE_H

#include "treeitem.h"
#include "treemodelbuilderinterface.h"

class TreeModelBulderFromExecutable : public TreeModelBuilderInterface
{
public:
    TreeModelBulderFromExecutable()  = default;
    ~TreeModelBulderFromExecutable() = default;

public:
    std::unique_ptr<TreeModel> buildModel(std::vector<std::unique_ptr<ADTExecutable>> elements) override;
};

#endif // TREEMODELBULDERFROMEXECUTABLE_H
