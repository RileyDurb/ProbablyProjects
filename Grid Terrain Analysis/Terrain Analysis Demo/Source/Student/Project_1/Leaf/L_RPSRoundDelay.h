#pragma once
#include "BehaviorNode.h"

class L_RPSRoundDelay : public BaseNode<L_RPSRoundDelay>
{
public:
    L_RPSRoundDelay();

protected:
    float timer;
    float roundDelay;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};