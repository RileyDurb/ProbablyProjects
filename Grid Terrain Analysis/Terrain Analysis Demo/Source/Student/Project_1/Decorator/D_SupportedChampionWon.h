#pragma once
#include "BehaviorNode.h"

class D_SupportedChampionWon : public BaseNode<D_SupportedChampionWon>
{
public:
    D_SupportedChampionWon();
    Agent::EndRPSBehaviour parentStatus;

protected:

    virtual void on_enter() override;
    virtual void on_update(float dt) override;

};