#pragma once
#include "BehaviorNode.h"

class L_CheckRangeBattleStart : public BaseNode<L_CheckRangeBattleStart>
{
protected:
    virtual void on_update(float dt);
};