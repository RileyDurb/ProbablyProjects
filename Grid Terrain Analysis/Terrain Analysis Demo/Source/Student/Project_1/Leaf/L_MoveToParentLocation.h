#pragma once
#include "BehaviorNode.h"

class L_MoveToParentLocation : public BaseNode<L_MoveToParentLocation>
{
public:
    //L_MoveToParentLocation();
    Vec3 targetPoint;
    float timer;
    float colorChangeTimer = 0.2f;
    bool isBlue = false;

protected:

    virtual void on_enter() override;
    virtual void on_update(float dt) override;

};