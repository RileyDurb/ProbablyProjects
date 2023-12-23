#pragma once
#include "BehaviorNode.h"
#include "Misc/NiceTypes.h"

class L_PatrolToNextCorner : public BaseNode<L_PatrolToNextCorner>
{
protected:
    virtual void on_enter() override;
    virtual void on_update(float dt) override;

    Vec3 targetPoint = Vec3(0, 0, 0);

private:
};