#include <pch.h>
#include "D_HasPartner.h"

D_HasPartner::D_HasPartner()
{}

void D_HasPartner::on_enter()
{
    BehaviorNode::on_enter();
}

void D_HasPartner::on_update(float dt)
{
    if (agent->get_blackboard().get_value<bool>("Has Partner") == false)
    {
        if (agents->HasPartnerRequest(agent))
        {
            // Accepts new partner, and sets it
            Agent* newPartner = agents->AcceptPartnerRequest(agent);
            auto& bb = agent->get_blackboard();
            bb.set_value("Partner Agent", newPartner);
            bb.set_value("Has Partner", true);

            on_success(); // Now that we have a partner, return success
            return;
        }

        // No partner, continues
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
    else
    {
        // Clears any partner requests for it
        if (agents->HasPartnerRequest(agent))
        {
            // Accepts new partner, and sets it
            agents->AcceptPartnerRequest(agent);

            on_success(); // Now that we have a partner, return success
            return;
        }
        on_success();
    }
}
