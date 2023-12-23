#include <pch.h>
#include "L_ApplyCurrVelocity.h"

L_ApplyCurrVelocity::L_ApplyCurrVelocity()
{}

void L_ApplyCurrVelocity::on_enter()
{


	BehaviorNode::on_leaf_enter();
}

void L_ApplyCurrVelocity::on_update(float dt)
{
    Vec3 oldPos = agent->get_position(); // saves current position

    // Sets new position based on velocity
    Vec3 currVelocity = agent->get_velocity();
    currVelocity.Normalize();
    currVelocity *= (agent->get_movement_speed() * dt);
    agent->set_position(agent->get_position() + currVelocity);

    // Sets direction to face velocity
    Vec3 newPos = agent->get_position();
    agent->set_yaw(MathHelpers::GetFacingYaw(oldPos, newPos));

    on_success();

    display_leaf_text();
}
