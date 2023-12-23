#include <pch.h>
#include "D_RepeatThreeTimes.h"

D_RepeatThreeTimes::D_RepeatThreeTimes() : counter(0)
{}

void D_RepeatThreeTimes::on_enter()
{
    counter = 0;
    BehaviorNode::on_enter();
}

void D_RepeatThreeTimes::on_update(float dt)
{
    // repeat the child until 3 successes, stop on any failure
    BehaviorNode *child = children.front();

    child->tick(dt);

    if (child->succeeded() == true)
    {
        ++counter;

        if (counter == 3)
        {
            on_success();
        }
        else
        {
            child->set_status(NodeStatus::READY);
        }
    }
    else if (child->failed() == true)
    {
        on_failure();
    }
}
