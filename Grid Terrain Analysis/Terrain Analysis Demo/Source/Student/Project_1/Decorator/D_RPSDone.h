#pragma once
#include "BehaviorNode.h"

class D_RPSDone : public BaseNode<D_RPSDone>
{
public:
    D_RPSDone();

protected:

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};