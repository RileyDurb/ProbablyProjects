#include <pch.h>
#include "L_CheckRangeBattleStart.h"
#include "../Agent/BehaviorAgent.h"

const float battleRange = 20.0f;
void L_CheckRangeBattleStart::on_update(float dt)
{
    const std::vector<Agent*>& allAgents = agents->get_all_agents_by_type("Combatant");

    // and our agent's position
    const auto& currPos = agent->get_position();

    float shortestDistance = std::numeric_limits<float>().max();
    Vec3 closestPoint;
    bool targetFound = false;
    Agent* foundPartner = NULL;

    for (int i = 0; i < allAgents.size(); i++)
    {
        if (allAgents[i] != agent) // If not our agent
        {
            float distance = currPos.Distance(allAgents[i]->get_position(), currPos);
            if (distance <= battleRange)
            {
                if (shortestDistance > distance)
                {
                    closestPoint = allAgents[i]->get_position();
                    shortestDistance = distance;
                    foundPartner = allAgents[i];
                    targetFound = true;
                }
            }
        }
    }

    if (targetFound == true)
    {
        // Sets personal blackboard
        auto& bb = agent->get_blackboard();

        bb.set_value("Has Partner", true);

        bb.set_value("Partner Agent", foundPartner);

        if (foundPartner)
        // Sets partner request for opponent
        agents->AddPartnerRequest(agent, foundPartner->get_id());

        // gets rid of partner request, if any
        if (agents->HasPartnerRequest(agent))
        {
            agents->AcceptPartnerRequest(agent);
        }

        on_success();
    }
    else
    {
        on_failure();
    }

    display_leaf_text();
}
