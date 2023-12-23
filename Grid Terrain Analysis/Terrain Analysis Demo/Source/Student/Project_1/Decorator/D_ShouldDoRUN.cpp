#include <pch.h>
#include "D_ShouldDoRUN.h"

D_ShouldDoRUN::D_ShouldDoRUN() : delay(0.0f)
{}

void D_ShouldDoRUN::on_enter()
{
    delay = RNG::range(1.0f, 2.0f);

    BehaviorNode::on_enter();
}

void D_ShouldDoRUN::on_update(float dt)
{
    auto& bb = agent->get_blackboard();

    Agent::EndRPSBehaviour afterBattleState = agent->get_end_rps_behaviour();
    if (afterBattleState == Agent::EndRPSBehaviour::RUN) // If in the run state
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
