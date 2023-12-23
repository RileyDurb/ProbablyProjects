#include <pch.h>
#include "D_SupportedChampionWon.h"

D_SupportedChampionWon::D_SupportedChampionWon()
    : parentStatus(Agent::EndRPSBehaviour::None)
{}

void D_SupportedChampionWon::on_enter()
{
    //delay = RNG::range(1.0f, 2.0f);

    BehaviorNode::on_enter();
}

void D_SupportedChampionWon::on_update(float dt)
{
    if (parentStatus == Agent::EndRPSBehaviour::None)
    {
        parentStatus = agent->get_parent()->get_end_rps_behaviour();

        if (parentStatus == Agent::EndRPSBehaviour::None) // Return until a status is selected
        {
            return;
        }
    }
    if (parentStatus == Agent::EndRPSBehaviour::Chase) // If in the chase state (won rps)
    {
        // Go down this path, update as normal
        BehaviorNode *child = children.front();

        child->tick(dt);

        // assume same status as child
        set_status(child->get_status());
        set_result(child->get_result());
    }
    else // In a different state, don't pick this one
    {
        on_failure();
    }
}
