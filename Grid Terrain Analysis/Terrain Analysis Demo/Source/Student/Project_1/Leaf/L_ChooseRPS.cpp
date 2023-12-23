#include <pch.h>
#include "L_ChooseRPS.h"

void L_ChooseRPS::on_enter()
{
	int rpsChoice = RNG::d3();

	agent->set_rps_choice(static_cast<Agent::RPSChoices>(rpsChoice));

	if (static_cast<Agent::RPSChoices>(rpsChoice) == Agent::RPSChoices::Rock)
	{
		agent->set_color(Vec3(0.0f, 0.0f, 0.0f));
		agent->set_scaling(0.7f);
		agent->set_pitch(PI / 2);
	}
	else if (static_cast<Agent::RPSChoices>(rpsChoice) == Agent::RPSChoices::Paper)
	{
		agent->set_color(Vec3(1.0f, 1.0f, 1.0f));
		agent->set_scaling(1.2f);
		agent->set_pitch(PI / 2);
	}
	else if (static_cast<Agent::RPSChoices>(rpsChoice) == Agent::RPSChoices::Scissors)
	{
		agent->set_color(Vec3(1.0f, 0.0f, 0.0f));
		agent->set_scaling(1.3f);
		agent->set_pitch(PI / 4);
	}

	BehaviorNode::on_leaf_enter();
	on_success();

#ifdef _DEBUG
	std::string choiceName;
	if (rpsChoice == 1)
	{
		choiceName = "Rock";
	}
	else if (rpsChoice == 2)
	{
		choiceName = "Paper";
	}
	else if (rpsChoice == 3)
	{
		choiceName = "Scissors";
	}

	std::wstring debugChoice = std::wstring(choiceName.begin(), choiceName.end());
	agent->add_debug_text(debugChoice);
#endif
}