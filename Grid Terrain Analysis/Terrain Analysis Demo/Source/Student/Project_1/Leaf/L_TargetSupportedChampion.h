#pragma once
#include "BehaviorNode.h"

class L_TargetSupportedChampion : public BaseNode<L_TargetSupportedChampion>
{
public:
    L_TargetSupportedChampion();

protected:

    virtual void on_enter() override;
};