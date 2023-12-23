#include <pch.h>
#include "L_PlayRPS.h"
#include "Projects/ProjectOne.h"


L_PlayRPS::L_PlayRPS()
    : timer(1.0f)
    , handOffset(6.0f)
    , handHeight(20.0f)
{}

void L_PlayRPS::on_enter()
{
    //timer = RNG::range(1.0f, 2.0f);

    Agent* partner = agent->get_blackboard().get_value<Agent*>("Partner Agent");
    
    // Face partner
    Vec3 toPartner = partner->get_position() - agent->get_position();
    toPartner.Normalize();
    agent->set_yaw(std::atan2(toPartner.x, toPartner.z));

    if (agent->get_child() != NULL) // If alredy spawned a hand (probably because we tied before)
    {
        BehaviorNode::on_leaf_enter();
        return; // Return early, no spawning needed
    }
    // Spawns an RPS hand for itself
    BehaviorAgent* rpsHand = agents->create_behavior_agent("RPS Hand", BehaviorTreeTypes::RPSHand);

    // Sets up parent and child
    rpsHand->set_parent(agent);
    agent->set_child(rpsHand);

    // Sets spawn position
    Vec3 forwardVec = agent->get_forward_vector();
    forwardVec.Normalize();
    Vec3 spawnPos = agent->get_position() + (forwardVec * handOffset);
    spawnPos.y += handHeight;
    Vec3 rightVector = agent->get_right_vector();
    rightVector.Normalize();
    spawnPos += (rightVector * 3);
    rpsHand->set_position(spawnPos);

    // makes hand face partner
    rpsHand->set_yaw(MathHelpers::GetFacingYaw(rpsHand->get_position(), partner->get_position()));

    // Sets hand visual info
    rpsHand->set_scaling(1.0);
    rpsHand->set_color(Vec3(0.6f, 0.6f, 0.6f));
    rpsHand->set_pitch(180);

	BehaviorNode::on_leaf_enter();
}

void L_PlayRPS::on_update(float dt)
{
    //timer -= dt;
    auto & bb = agent->get_blackboard();
    Agent* partner = bb.get_value<Agent*>("Partner Agent");

    Agent* partnerHand = partner->get_child();
    if (partnerHand == NULL)
    {
        return; // partner hasn't gotten to on_enter for RPS yet, just return
    }

    Agent::RPSChoices partnerChoice = partnerHand->get_rps_choice();
    Agent* myHand = agent->get_child();
    Agent::RPSChoices myChoice = myHand->get_rps_choice();

    if (partnerChoice == Agent::RPSChoices::None ||
        myChoice == Agent::RPSChoices::None)
    {
        // Do nothing
        display_leaf_text();
        return;
    }
    else if (partnerChoice == myChoice)
    {
        on_failure(); // Fails, and should play rps again
        display_leaf_text();
        return;
    }

    //Begin the chase!
    if (myChoice == Agent::RPSChoices::Rock)
    {
        if (partnerChoice == Agent::RPSChoices::Scissors)
        {
            // Win
           agent->set_end_rps_behaviour(Agent::EndRPSBehaviour::Chase); // Win
        }
        else if (partnerChoice == Agent::RPSChoices::Paper)
        {
            agent->set_end_rps_behaviour(Agent::EndRPSBehaviour::RUN); // Loose
        }
    }
    else if (myChoice == Agent::RPSChoices::Paper)
    {
        if (partnerChoice == Agent::RPSChoices::Rock)
        {
            agent->set_end_rps_behaviour(Agent::EndRPSBehaviour::Chase); // Win
        }
        else if (partnerChoice == Agent::RPSChoices::Scissors)
        {
            agent->set_end_rps_behaviour(Agent::EndRPSBehaviour::RUN); // Loose
        }
    }
    else if (myChoice == Agent::RPSChoices::Scissors)
    {
        if (partnerChoice == Agent::RPSChoices::Paper)
        {
            agent->set_end_rps_behaviour(Agent::EndRPSBehaviour::Chase); // Win
        }
        else if (partnerChoice == Agent::RPSChoices::Rock)
        {
            agent->set_end_rps_behaviour(Agent::EndRPSBehaviour::RUN); // Loose
        }
    }
    on_success();
    
    

    //if (timer < 0.0f)
    //{
    //    on_success();
    //}

    display_leaf_text();
}
