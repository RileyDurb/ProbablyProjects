#pragma once
#include "BehaviorNode.h"

class D_HasChampion : public BaseNode<D_HasChampion>
{
public:
    D_HasChampion();

protected:

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};