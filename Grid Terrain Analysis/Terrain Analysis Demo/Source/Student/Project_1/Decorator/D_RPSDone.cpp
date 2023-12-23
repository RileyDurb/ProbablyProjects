#include <pch.h>
#include "D_RPSDone.h"

D_RPSDone::D_RPSDone()
{}

void D_RPSDone::on_enter()
{
    BehaviorNode::on_enter();
}

void D_RPSDone::on_update(float dt)
{
    Agent* parent = agent->get_parent();
    Agent::EndRPSBehaviour parentBehaviour = parent->get_end_rps_behaviour();
    if (parentBehaviour == Agent::EndRPSBehaviour::None)
    {
        // RPS still going, continues
        BehaviorNode* child = children.front();

        child->tick(dt);

        // assume same status as child
        set_status(child->get_status());
        // Conditionally assumes child result
        if (child->get_result() == NodeResult::SUCCESS) // If child succeeds
        {
            set_result(NodeResult::FAILURE); // Set result to failure, we're still in rps with the parent, so we don't want to continue

        }
        else
        {
            set_result(child->get_result());
        }
    }
    else
    {
        on_success();
    }
}
