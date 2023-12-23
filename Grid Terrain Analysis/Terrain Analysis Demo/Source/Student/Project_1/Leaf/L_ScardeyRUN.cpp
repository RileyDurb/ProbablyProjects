#include <pch.h>
#include "L_ScardeyRUN.h"

L_ScardeyRUN::L_ScardeyRUN()
    : timer(0.0f)
    , runTime(5.0f)
    , fleeVector()
{}

void L_ScardeyRUN::on_enter()
{
    timer = runTime;
    
    // Calculates flee vector (does in init, because our parent could become null
    Agent* parent = agent->get_parent();
    Vec3 fleeFromParentVector = agent->get_position() - parent->get_position();
    fleeFromParentVector.Normalize();
    fleeVector = fleeFromParentVector;

    agent->set_yaw(MathHelpers::GetFacingYaw(parent->get_position(), agent->get_position()));


	BehaviorNode::on_leaf_enter();
}

void L_ScardeyRUN::on_update(float dt)
{

    Vec3 checkNewPos = agent->get_position() + fleeVector * (agent->get_movement_speed() * dt);
    if (terrain->is_valid_grid_position(terrain->get_grid_position(checkNewPos))) // Ensures new pos is inside map
    {
        agent->set_position(checkNewPos);
    }

    timer -= dt;

    if (timer < 0.0f)
    {
        on_success();
    }

    display_leaf_text();
}
