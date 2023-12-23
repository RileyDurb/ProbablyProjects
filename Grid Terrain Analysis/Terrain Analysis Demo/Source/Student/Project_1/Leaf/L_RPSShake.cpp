#include <pch.h>
#include "L_RPSShake.h"

L_RPSShake::L_RPSShake() 
    : timer(0.0f)
    , shakeDistance(2.5f)
    , shakeTime(.3f)
    , shakeState(ShakeStates::InitialRise)
    , initialPos()
{}

void L_RPSShake::on_enter()
{
    //timer = RNG::range(1.0f, 2.0f);

    //Agent* partner = agent->get_blackboard().get_value<Agent*>("Partner Agent");
    //
    //// Face partner
    //Vec3 toPartner = partner->get_position() - agent->get_position();
    //toPartner.Normalize();
    //agent->set_yaw(std::atan2(toPartner.x, toPartner.z));
    
    shakeState = ShakeStates::InitialRise;
    initialPos = agent->get_position();

    agent->set_movement_speed((shakeDistance) / shakeTime); // Sets velocity to cover the shake distance in the given shake time
    agent->set_pitch(180);
    agent->set_color(Vec3(0.6f, 0.6f, 0.6f));
    agent->set_scaling(1.0);
    //rpsHand->set_yaw(MathHelpers::GetFacingYaw(rpsHand->get_position(), partner->get_position()));

    // Sets hand visual info
    //rpsHand->set_scaling(1.0);
    //rpsHand->set_pitch(180);

	BehaviorNode::on_leaf_enter();
}

void L_RPSShake::on_update(float dt)
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
    //else
    //{
    //    const auto result = agent->move_toward_point(initialPos + Vec3(0,shakeDistance / 2, 0), dt);

    //    if (result == true)
    //    {
    //        shakeState = ShakeStates::InitialRise;
    //        
    //        on_success();
    //    }
    //}


    display_leaf_text();
}
