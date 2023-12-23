#include <pch.h>
#include "L_Arrival.h"

L_Arrival::L_Arrival()
    : timer(0.0f)
    , moveRight(true)
{}

void L_Arrival::on_enter()
{
    timer = RNG::range(1.0f, 2.0f);

    //moveRight = RNG::coin_toss(); // Decides if moving to the right or left of parent's future velocity

	BehaviorNode::on_leaf_enter();
}

void L_Arrival::on_update(float dt)
{
    auto& bb = agent->get_blackboard();

    Agent* partner = bb.get_value<Agent*>("Partner Agent");
    //// Checks if caught
    //if (Vec3::Distance(agent->get_position(), partner->get_position()) <= agents->GetCatchRange())
    //{

    //    // Kills it's, ahem...
    //    agents->destroy_agent(agent->get_child());

    //    // Dies
    //    agents->destroy_agent(agent);

    //    // Spawns a replacement
    //    agents->create_behavior_agent("Combatant", BehaviorTreeTypes::TestCombatant);

    //    on_failure();
    //    display_leaf_text();
    //    return; // Already caught, so don't run, and return early
    //}

    // Flees
    Vec3 fleeVector = agent->get_position() - partner->get_position();

    fleeVector.Normalize();

    Vec3 checkNewPos = agent->get_position() + fleeVector * (agent->get_movement_speed() * dt);
    if (terrain->is_valid_grid_position(terrain->get_grid_position(checkNewPos))) // Ensures new pos is inside map
    {
        agent->set_position(checkNewPos);

        agent->set_yaw(MathHelpers::GetFacingYaw(agent->get_position(), partner->get_position()));
    }

    //timer -= dt;

    //if (timer < 0.0f)
    //{
    //    on_success();
    //}

    display_leaf_text();
}
