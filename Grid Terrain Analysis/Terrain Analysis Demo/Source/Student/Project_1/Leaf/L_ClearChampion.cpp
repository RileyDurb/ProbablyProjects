#include <pch.h>
#include "L_ClearChampion.h"

L_ClearChampion::L_ClearChampion()
{}

void L_ClearChampion::on_enter()
{
	agent->set_parent(NULL);
	on_success();

	//BehaviorNode::on_leaf_enter();
}
