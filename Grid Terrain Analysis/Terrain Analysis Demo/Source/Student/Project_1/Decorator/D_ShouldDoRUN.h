#pragma once
#include "BehaviorNode.h"

class D_ShouldDoRUN : public BaseNode<D_ShouldDoRUN>
{
public:
    D_ShouldDoRUN();

protected:
    float delay;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};