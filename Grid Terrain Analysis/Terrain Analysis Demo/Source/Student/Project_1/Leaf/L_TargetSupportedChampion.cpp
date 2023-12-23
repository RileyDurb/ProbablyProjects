#include <pch.h>
#include "L_TargetSupportedChampion.h"

L_TargetSupportedChampion::L_TargetSupportedChampion()
{}

void L_TargetSupportedChampion::on_enter()
{

    // Sets velocity to be towards parent
    Agent* parent = agent->get_parent();
    Vec3 parentPos = parent->get_position();
    Vec3 directionToParent = parentPos - agent->get_position();
    directionToParent.Normalize();
    agent->set_velocity(directionToParent);

    on_success(); // We're done, set success
}