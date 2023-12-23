#pragma once
#include "BehaviorNode.h"

class L_CheckRangeForFight : public BaseNode<L_CheckRangeForFight>
{
protected:
    virtual void on_update(float dt);
};