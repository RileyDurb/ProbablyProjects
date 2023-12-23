#pragma once
#include "BehaviorNode.h"

class L_ApplyCurrVelocity : public BaseNode<L_ApplyCurrVelocity>
{
public:
    L_ApplyCurrVelocity();

protected:

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};