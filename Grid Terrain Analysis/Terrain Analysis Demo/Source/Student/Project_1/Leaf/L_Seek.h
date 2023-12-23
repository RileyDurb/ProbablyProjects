#pragma once
#include "BehaviorNode.h"

class L_Arrival : public BaseNode<L_Arrival>
{
public:
    L_Arrival();

protected:
    float timer;
    bool moveRight;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};