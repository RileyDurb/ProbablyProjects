#pragma once
#include "BehaviorNode.h"

class L_CheckRangeForChampion : public BaseNode<L_CheckRangeForChampion>
{

protected:
    virtual void on_update(float dt);
};