#include <pch.h>
#include "L_SetFindRange.h"

L_SetFindRange::L_SetFindRange()
{}

void L_SetFindRange::on_enter()
{
    auto& bb = agent->get_blackboard();

    if (agents->GetNumOngoingFights() > 0) // if any fights are going on
    {
        bb.set_value("Fight Search Range", 70.0f); // Set to high range
    }
    else
    {
        bb.set_value("Fight Search Range", 30.0f); // Set to low range

    }

    on_success();
    display_leaf_text();
}

