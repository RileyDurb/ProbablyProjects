#include <pch.h>
#include "D_HasChampion.h"

D_HasChampion::D_HasChampion()
{}

void D_HasChampion::on_enter()
{

    BehaviorNode::on_enter();
}

void D_HasChampion::on_update(float dt)
{
    if (agent->get_parent() == NULL)
    {
        agent->set_color(Vec3(1.0f, 1.0f, 0.2f)); // Sets to spectator color
        agent->set_scaling(2.0f);

        // No champion, continues
        BehaviorNode* child = children.front();

        child->tick(dt);

        // assume same status as child
        set_status(child->get_status());
        // Conditionally assumes child result
        if (child->get_result() == NodeResult::SUCCESS) // If child succeds
        {
            set_result(NodeResult::FAILURE); // Set result to failure, we don't have a partner

        }
        else
        {
            set_result(child->get_result());
        }
    }
    else // Parent not null, so a champion has been set
    {
        on_success();
    }
}
