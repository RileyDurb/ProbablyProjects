#pragma once
#include "BehaviorNode.h"

class D_HasPartner : public BaseNode<D_HasPartner>
{
public:
    D_HasPartner();

protected:

    virtual void on_enter() override;
    virtual void on_update(float dt) override;
};