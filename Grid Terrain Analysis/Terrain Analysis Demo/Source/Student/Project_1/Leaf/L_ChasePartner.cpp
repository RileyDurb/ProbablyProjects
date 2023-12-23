#include <pch.h>
#include "L_ChasePartner.h"
#include "Projects/ProjectOne.h" // Behaviour tree types


L_ChasePartner::L_ChasePartner() : timer(0.0f)
{}

void L_ChasePartner::on_enter()
{
    //timer = RNG::range(1.0f, 2.0f);
    agents->SetNumOngoingFights(agents->GetNumOngoingFights() + 1); // increments fight counter

	BehaviorNode::on_leaf_enter();
}

void L_ChasePartner::on_update(float dt)
{
    //timer -= dt;

    auto& bb = agent->get_blackboard();
    Agent* partner = bb.get_value<Agent*>("Partner Agent");

    // Checks if caught partner
    if (Vec3::Distance(agent->get_position(), partner->get_position()) <= agents->GetCatchRange())
    {
        auto& bb = agent->get_blackboard();

        // Destroys partner
        agents->destroy_agent(bb.get_value<Agent*>("Partner Agent")->get_child());
        agents->destroy_agent(bb.get_value<Agent*>("Partner Agent"));

        // Resets partner data
        bb.set_value("Partner Agent", NULL);
        bb.set_value("Has Partner", false);

        // Destroys hand, done using it
        agents->destroy_agent(agent->get_child()); 
        agent->set_child(NULL);

        // Spawns new opponent
        BehaviorAgent* newOpponent = agents->create_behavior_agent("Combatant", BehaviorTreeTypes::TestCombatant);
        newOpponent->set_movement_speed(agents->GetRPSCombatantBaseSpeed());

        // Resets end of RPS behaviour
        agent->set_end_rps_behaviour(Agent::EndRPSBehaviour::None);

        // Decreases fight count
        agents->SetNumOngoingFights(agents->GetNumOngoingFights() - 1);


        on_success();
        display_leaf_text();
        return; // We're done, return early
        //bb.set_value("After RPS State")

        //// Spawns a replacement
        //agents->create_behavior_agent("Combatant", BehaviorTreeTypes::TestCombatant);
    }

    Vec3 partnerPos = partner->get_position();

    bool result = agent->move_toward_point_on_map(partnerPos, dt);

    display_leaf_text();
}
