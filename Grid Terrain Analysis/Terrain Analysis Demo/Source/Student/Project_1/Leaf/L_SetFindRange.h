#pragma once
#include "BehaviorNode.h"

class L_SetFindRange : public BaseNode<L_SetFindRange>
{
public:
    L_SetFindRange();

protected:

    virtual void on_enter() override;
};