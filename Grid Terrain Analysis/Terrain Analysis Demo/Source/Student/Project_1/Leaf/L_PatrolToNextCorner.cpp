#include <pch.h>
#include "L_PatrolToNextCorner.h"
#include "Agent/BehaviorAgent.h"

void L_PatrolToNextCorner::on_enter()
{
    // set animation, speed, etc

    auto& bb = agent->get_blackboard();

    int patrolCorner = bb.get_value<int>("Patrol Corner");

    /*
    0 == upper left
    1 == upper right
    2 == lower right
    3 == lower left
    */

    if (patrolCorner == 0)
    {
        targetPoint = Vec3(0, 0, float(terrain->get_map_height()));
    }
    else if (patrolCorner == 1)
    {
        targetPoint = Vec3(float(terrain->get_map_width()), 0, float(terrain->get_map_height()));
    }
    else if (patrolCorner == 2)
    {
        targetPoint = Vec3(float(terrain->get_map_width()), 0, 0);
    }
    else if (patrolCorner == 3)
    {
        targetPoint = Vec3(0, 0, 0);
    }

    
    //agent->look_at_point(targetPoint);

	BehaviorNode::on_leaf_enter();
}

void L_PatrolToNextCorner::on_update(float dt)
{
    const auto result = agent->move_toward_point(targetPoint, dt);

    if (result == true)
    {
        auto& bb = agent->get_blackboard();

        // Sets next patrol corner
        int patrolCorner = bb.get_value<int>("Patrol Corner");
        if (patrolCorner == 3) // Lops if last corner
        {
            patrolCorner = 0;
        }
        else // Increments corner
        {
            patrolCorner++;
        }

        bb.set_value("Patrol Corner", patrolCorner);

        on_success();
    }

    display_leaf_text();
}
