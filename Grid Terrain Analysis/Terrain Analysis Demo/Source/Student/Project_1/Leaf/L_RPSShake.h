#pragma once
#include "BehaviorNode.h"

class L_RPSShake : public BaseNode<L_RPSShake>
{
public:
    enum class ShakeStates
    {
        InitialRise,
        Down,
        BackToCenter
    };
    L_RPSShake();

protected:
    float timer;
    float shakeDistance;
    float shakeTime;
    ShakeStates shakeState;
    Vec3 initialPos;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};