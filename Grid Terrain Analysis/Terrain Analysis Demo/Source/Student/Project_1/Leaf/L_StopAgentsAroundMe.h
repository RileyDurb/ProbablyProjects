#pragma once
#include "BehaviorNode.h"

class L_StopAgentsAroundMe : public BaseNode<L_StopAgentsAroundMe>
{
public:
    L_StopAgentsAroundMe();
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

    float stopRange;
};