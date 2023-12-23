#pragma once
#include "BehaviorNode.h"

class L_PlayRPS : public BaseNode<L_PlayRPS>
{
public:
    L_PlayRPS();

protected:
    float timer;
    float handOffset;
    float handHeight;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};