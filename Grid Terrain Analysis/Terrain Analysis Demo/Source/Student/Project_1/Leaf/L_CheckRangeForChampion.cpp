#include <pch.h>
#include "L_CheckRangeForChampion.h"
#include "../Agent/BehaviorAgent.h"

const float championRange = 30.0f;
void L_CheckRangeForChampion::on_update(float dt)
{
    const std::vector<Agent*>& allAgents = agents->get_all_agents_by_type("Combatant");

    // and our agent's position
    const auto& currPos = agent->get_position();

    float shortestDistance = std::numeric_limits<float>().max();
    Vec3 closestPoint;
    bool targetFound = false;
    Agent* foundChampion = NULL;

    for (int i = 0; i < allAgents.size(); i++)
    {
		float distance = currPos.Distance(allAgents[i]->get_position(), currPos);
		if (distance <= championRange)
		{
			if (shortestDistance > distance)
			{
				closestPoint = allAgents[i]->get_position();
				shortestDistance = distance;
				foundChampion = allAgents[i];
				targetFound = true;
			}
		}
    }

    if (targetFound == true)
    {
        // Sets champion as parent

        agent->set_parent(foundChampion);

        on_success();
    }
    else
    {
        on_failure();
    }

    display_leaf_text();
}
