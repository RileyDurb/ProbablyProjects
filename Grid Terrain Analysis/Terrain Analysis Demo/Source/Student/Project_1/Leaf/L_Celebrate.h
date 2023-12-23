#pragma once
#include "BehaviorNode.h"

class L_Celebrate : public BaseNode<L_Celebrate>
{
public:
    enum class ShakeStates
    {
        InitialRise,
        Down,
        BackToCenter
    };
    L_Celebrate();

protected:
    float timer;
    float shakeDistance;
    float shakeTime;
    ShakeStates shakeState;
    Vec3 initialPos;
    float initialMoveSpeed;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
    virtual void on_exit() override;
};