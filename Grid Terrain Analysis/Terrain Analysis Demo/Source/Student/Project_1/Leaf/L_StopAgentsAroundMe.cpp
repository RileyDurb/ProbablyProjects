#include <pch.h>
#include "L_StopAgentsAroundMe.h"
#include "../Agent/BehaviorAgent.h"


L_StopAgentsAroundMe::L_StopAgentsAroundMe()
    :  stopRange(30.0f)
{
}
void L_StopAgentsAroundMe::on_enter()
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
        if (distance <= stopRange)
        {
            // Kill and clean up


        }
    }

    agent->set_parent(NULL); // Clears parent so it can reset to patrolling
    on_success();
}
void L_StopAgentsAroundMe::on_update(float dt)
{
    

    //if (targetFound == true)
    //{
    //    // Sets champion as parent

    //    agent->set_parent(foundChampion);

    //    on_success();
    //}
    //else
    //{
    //    on_failure();
    //}

    display_leaf_text();
}
