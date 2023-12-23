#pragma once
#include "BehaviorNode.h"

class L_ScardeyRUN : public BaseNode<L_ScardeyRUN>
{
public:
    L_ScardeyRUN();

protected:
    float timer;
    float runTime;
    Vec3 fleeVector;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};