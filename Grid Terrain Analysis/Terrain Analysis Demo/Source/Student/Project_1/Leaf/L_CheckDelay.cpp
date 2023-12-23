#include <pch.h>
#include "L_CheckDelay.h"

L_CheckDelay::L_CheckDelay() : timer(0.0f)
{}

const float delayTime = 5.0f;

void L_CheckDelay::on_enter()
{
    timer = delayTime;

	BehaviorNode::on_leaf_enter();
}

void L_CheckDelay::on_update(float dt)
{
    timer -= dt;

    if (timer < 0.0f)
    {
        on_success();
    }

    display_leaf_text();
}
