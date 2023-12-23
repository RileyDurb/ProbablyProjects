#include <pch.h>
#include "L_Celebrate.h"

L_Celebrate::L_Celebrate() 
    : timer(0.0f)
    , shakeDistance(2.5f)
    , shakeTime(0.1f)
    , shakeState(ShakeStates::InitialRise)
    , initialPos()
    , initialMoveSpeed(0.0f)
{}

void L_Celebrate::on_enter()
{
    
    shakeState = ShakeStates::InitialRise;
    initialPos = agent->get_position();

    initialMoveSpeed = agent->get_movement_speed();
    agent->set_movement_speed((shakeDistance) / shakeTime); // Sets velocity to cover the shake distance in the given shake time
    //agent->set_pitch(180);
    //agent->set_color(Vec3(0.6f, 0.6f, 0.6f));
    //agent->set_scaling(1.0);

	BehaviorNode::on_leaf_enter();
}

void L_Celebrate::on_update(float dt)
{
    timer -= dt;
    
    if (shakeState == ShakeStates::InitialRise)
    {
        const auto result = agent->move_toward_point_no_yaw(initialPos + Vec3(0, shakeDistance, 0), dt);

        if (result == true)
        {
            shakeState = ShakeStates::Down;
        }

    }
    else if (shakeState == ShakeStates::Down)
    {
        const auto result = agent->move_toward_point_no_yaw(initialPos, dt);

        if (result == true)
        {
            shakeState = ShakeStates::InitialRise;

            on_success();
        }
    }


    display_leaf_text();
}

void L_Celebrate::on_exit()
{
    agent->set_movement_speed(initialMoveSpeed);
}
