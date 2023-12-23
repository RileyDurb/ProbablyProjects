#include <pch.h>
#include "L_CheckRangeForFight.h"
#include "../Agent/BehaviorAgent.h"

void L_CheckRangeForFight::on_update(float dt)
{
    auto& bb = agent->get_blackboard();
    float fightCheckRange = bb.get_value<float>("Fight Search Range");

    const std::vector<Agent*>& allAgents = agents->get_all_agents_by_type("Combatant");

    // and our agent's position
    const auto& currPos = agent->get_position();

    float shortestDistance = std::numeric_limits<float>().max();
    Vec3 closestPoint;
    bool targetFound = false;
    Agent* foundFighter = NULL;

    for (int i = 0; i < allAgents.size(); i++)
    {
        Agent* currCheckAgent = allAgents[i];
		float distance = currPos.Distance(currCheckAgent->get_position(), currPos);
		if (distance <= fightCheckRange)
		{
            if (currCheckAgent->get_child() != NULL) // if child is not null, then it's fighting
            {
                if (shortestDistance > distance)
                {
                    closestPoint = allAgents[i]->get_position();
                    shortestDistance = distance;
                    foundFighter = allAgents[i];
                    targetFound = true;
                }
            }

		}
    }

    if (targetFound == true)
    {
        // Sets fighter as parent

        agent->set_parent(foundFighter);

        on_success();
    }
    else
    {
        on_failure();
    }

    display_leaf_text();
}
