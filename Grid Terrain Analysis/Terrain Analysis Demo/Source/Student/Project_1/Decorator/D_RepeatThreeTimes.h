#pragma once
#include "BehaviorNode.h"

class D_RepeatThreeTimes : public BaseNode<D_RepeatThreeTimes>
{
public:
    D_RepeatThreeTimes();

protected:
    unsigned counter;

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};