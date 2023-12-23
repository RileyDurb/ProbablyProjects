#pragma once
#include "BehaviorNode.h"

class L_SetHandPosition : public BaseNode<L_SetHandPosition>
{
public:
    L_SetHandPosition();

protected:
    //float timer;
    //bool moveRight;
    float handOffsetForward;
    float handHeight;
    float handOffsetRight;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};