#pragma once
#include "BehaviorNode.h"

class L_RUN : public BaseNode<L_RUN>
{
public:
    L_RUN();

protected:
    float timer;
    bool moveRight;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};