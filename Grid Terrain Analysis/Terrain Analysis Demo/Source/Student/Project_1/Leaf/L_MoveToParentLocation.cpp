#include <pch.h>
#include "L_MoveToParentLocation.h"

void L_MoveToParentLocation::on_enter()
{
    // set animation, speed, etc

    targetPoint = agent->get_parent()->get_position();
    //agent->look_at_point(targetPoint);

    agent->set_color(Vec3(0.0f, 0.0f, 1.0f));
    isBlue = true;
    timer = colorChangeTimer;

    BehaviorNode::on_leaf_enter();
}

void L_MoveToParentLocation::on_update(float dt)
{
    const auto result = agent->move_toward_point_on_map(targetPoint, dt);

    if (result == true)
    {
        agent->set_color(Vec3(1.0f, 1.0f, 0.0f)); // Sets back to yellow
        on_success();
    }

    timer -= dt;

    if (timer < 0.0f) // If timer up
    {
        // Switches color
        if (isBlue)
        {
            agent->set_color(Vec3(1.0f, 0.0f, 0.0f));
            isBlue = false;
        }
        else
        {
            agent->set_color(Vec3(0.0f, 0.0f, 1.0f));
            isBlue = true;
        }

        timer = colorChangeTimer;
    }


    display_leaf_text();
}
