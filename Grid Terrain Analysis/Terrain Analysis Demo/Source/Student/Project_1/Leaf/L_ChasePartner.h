#pragma once
#include "BehaviorNode.h"

class L_ChasePartner : public BaseNode<L_ChasePartner>
{
public:
    L_ChasePartner();

protected:
    float timer;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};