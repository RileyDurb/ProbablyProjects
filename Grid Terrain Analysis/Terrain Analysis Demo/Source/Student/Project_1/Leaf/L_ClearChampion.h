#pragma once
#include "BehaviorNode.h"

class L_ClearChampion : public BaseNode<L_ClearChampion>
{
public:
    L_ClearChampion();

protected:

    virtual void on_enter() override;
};