#include <pch.h>
#include "L_RPSRoundDelay.h"

L_RPSRoundDelay::L_RPSRoundDelay() 
    : timer(0.0f)
    , roundDelay(1.0f)
{}

void L_RPSRoundDelay::on_enter()
{
    timer = roundDelay;

	BehaviorNode::on_leaf_enter();
}

void L_RPSRoundDelay::on_update(float dt)
{
    timer -= dt;

    if (timer < 0.0f)
    {
        on_success();
    }

    display_leaf_text();
}
