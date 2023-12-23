#pragma once
#include "BehaviorNode.h"

class L_CheckDelay : public BaseNode<L_CheckDelay>
{
public:
    L_CheckDelay();

protected:
    float timer;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};