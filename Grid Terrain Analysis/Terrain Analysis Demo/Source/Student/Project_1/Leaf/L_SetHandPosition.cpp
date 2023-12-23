#include <pch.h>
#include "L_SetHandPosition.h"

L_SetHandPosition::L_SetHandPosition()
    //: timer(0.0f)
    //, moveRight(true)
    : handOffsetForward(2.0f)
    , handHeight(25.0f)
    , handOffsetRight(3.0f)
{}

void L_SetHandPosition::on_enter()
{
    //timer = RNG::range(1.0f, 2.0f);

    //moveRight = RNG::coin_toss(); // Decides if moving to the right or left of parent's future velocity

	BehaviorNode::on_leaf_enter();
}

void L_SetHandPosition::on_update(float dt)
{
    auto& bb = agent->get_blackboard();

    Agent* parent = agent->get_parent();
    Vec3 parentForward = parent->get_forward_vector();
    parentForward.Normalize();

    Vec3 newPos = parent->get_position() + (parentForward * handOffsetForward); // Sets position to an xy offset of the paren's front

    newPos.y += handHeight; // Sets height
    Vec3 rightVector = parent->get_right_vector();
    rightVector.Normalize();
    newPos += (rightVector * handOffsetRight); // Offsets to the right
    agent->set_position(newPos);

    // makes hand face partner
    agent->set_yaw(MathHelpers::GetFacingYaw(parent->get_position(), agent->get_position()));

    on_success();

    display_leaf_text();
}
