#include "FLBarrierManager.h"
//------------------------------------------------------------------------------
//
// File Name:	FLBarrierManager.h
// Author(s):	Riley Durbin 
// Class: GAM 150+
//
// Copyright ? 2023 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FLBarrierManager.h"
#include "FLEngine.h"
#include "BehaviourTrapBarrier.h"
#include "FLEventList.h"
#include "FLInput.h"
#include "BehaviourEnemy.h" // getting each enemy in manager
#include "FLSound.h"
#include "BehaviourMapModifier.h" // Resetting barriers

void FLBarrierManager::Init()
{
	EventList::BarrierTriggered().SubscribeMember(FLBarrierManager, ActivateBarrierGroup, "ActivateBarrierGroup");
	EventList::RestartMainGameEvent().SubscribeMember(FLBarrierManager, ResetActiveBarrierGroup, "ResetActiveBarrierGroup");
}

void FLBarrierManager::Update()
{
	if (mActiveBarrierGroupID != static_cast<int>(BarrierNums::NoActiveGroup))
	{
		if (mBarrierGroups[mActiveBarrierGroupID].mEnemies.empty() == true) // If all enemies have been killed
		{
			DeactivateBarrierGroup(mActiveBarrierGroupID); // Deactivates the current barrier group
		}
	}
}

void FLBarrierManager::Shutdown()
{
	EventList::BarrierTriggered().Unsubscribe("ActivateBarrierGroup");
	EventList::RestartMainGameEvent().Unsubscribe("ResetActiveBarrierGroup");
}

void FLBarrierManager::ActivateBarrierGroup(int barrierID)
{
	if (mBarrierGroups.contains(barrierID) == false) // Ensures barrier group exists
	{
		Trace::log("Tried to activate barrier group of ID: " + std::to_string(barrierID) + ", but no barrier group exists");
		return;
	}

	Scene* mainScene = FLEngine::Instance().Get<FLSceneManager>()->GetScene("Main Game"); // Gets the main scene

	if (mainScene)
	{
		ECS* mainGameEcs = mainScene->GetECS(); // Gets the ecs of the main scene
		
		if (mainGameEcs)
		{
			// Triggers each barrier
			for (EntityID barrierID : mBarrierGroups[barrierID].mBarriers)
			{
				if (mainGameEcs->HasComponent<BehaviourTrapBarrier>(barrierID))
				{
					BehaviourTrapBarrier& barrier = mainGameEcs->GetComponentFrommanager<BehaviourTrapBarrier>(barrierID);

					barrier.TriggerBarrier();
				}
			}

			mActiveBarrierGroupID = barrierID; // Saves active barrier group ID

			// Subscribes to each enemy that will need to be killed

			std::unordered_map<EntityID, BehaviourEnemy>& enemies = mainGameEcs->GetComponentManager<BehaviourEnemy>().GetComponents();

			for (auto& enemy : enemies) // Checks each enemy
			{
				if (EnemyInBarrierGroup(enemy.second, barrierID))
				{
					// Don't need to unsubscribe, because enemies die after calling
					enemy.second.DeathEvent().SubscribeMember(FLBarrierManager, RemoveMeFromBarrierGroup, "RemoveEnemy" + std::to_string(enemy.first) + "FromBarrierGroup"); 
					mBarrierGroups[barrierID].mEnemies.push_back(enemy.first); // Adds enemy to the group
				}
			}
		}
	}

	// Audio: Switches to the battle state
	FLSound::PostEvent("TO_BATTLE_BGM");
}

void FLBarrierManager::DeactivateBarrierGroup(int barrierID)
{
	if (mBarrierGroups.contains(barrierID) == false) // Ensures barrier group exists
	{
		Trace::log("Tried to activate barrier group of ID: " + std::to_string(barrierID) + ", but no barrier group exists");
		return;
	}

	Scene* mainScene = FLEngine::Instance().Get<FLSceneManager>()->GetScene("Main Game"); // Gets the main scene

	if (mainScene)
	{
		ECS* mainGameEcs = mainScene->GetECS(); // Gets the ecs of the main scene

		if (mainGameEcs)
		{
			// Deactivates each barrier in group
			for (EntityID barrierID : mBarrierGroups[barrierID].mBarriers)
			{
				if (mainGameEcs->HasComponent<BehaviourTrapBarrier>(barrierID))
				{
					BehaviourTrapBarrier& barrier = mainGameEcs->GetComponentFrommanager<BehaviourTrapBarrier>(barrierID);

					barrier.DeactivateBarrier();
				}
			}

			mActiveBarrierGroupID = static_cast<int>(BarrierNums::NoActiveGroup); // Clears saved active barrier ID
		}
	}

	// Audio: Switches to the explore
	FLSound::PostEvent("TO_EXPLORE_BGM");
}

void FLBarrierManager::AddBarrier(Entity barrierObj)
{
	BehaviourTrapBarrier& barrier = barrierObj.Get<BehaviourTrapBarrier>();

	if (barrier.GetBarrierID() < 0) // Ensures barrier ID is valid
	{
		Trace::log("Tried to add a barrier of ID " + std::to_string(barrier.GetBarrierID()) + ". Invalid becsause it is less than 0");
		return;
	}

	if (mBarrierGroups.contains(barrier.GetBarrierID())) // if group already exists
	{
		mBarrierGroups[barrier.GetBarrierID()].mBarriers.push_back(barrierObj.ID()); // adds to the group
	}
	else
	{
		mBarrierGroups.emplace(barrier.GetBarrierID(), BarrierGroup({barrierObj.ID()})); // creates a new group with this entity as the first
	}
}

void FLBarrierManager::AddEnemySection(glm::vec2 position, glm::vec2 scale, int barrierID)
{
	if (barrierID < 0) // Ensures barrier ID is valid
	{
		Trace::log("Tried to add a barrier of ID " + std::to_string(barrierID) + ". Invalid becsause it is less than 0");
		return;
	}

	if (mBarrierGroups.contains(barrierID)) // if group already exists
	{
		mBarrierGroups[barrierID].mEnemySections.push_back(EnemySection(position, scale)); // adds to the group
	}
	else
	{
		mBarrierGroups.emplace(barrierID, BarrierGroup({}, { EnemySection(position, scale) }, {})); // creates a new group with this entity as the first
	}
}

void FLBarrierManager::ClearBarriers()
{
	mBarrierGroups.clear();

	mActiveBarrierGroupID = static_cast<int>(BarrierNums::NoActiveGroup);
}

bool FLBarrierManager::EnemyInBarrierGroup(BehaviourEnemy& enemy, int barrierID)
{
	if (mBarrierGroups.contains(barrierID) == false)
	{
		Trace::log("Tried to check if enemy w/ ID: " + std::to_string(enemy.ID()) + " was in barrier group of ID " + std::to_string(barrierID) + ", but barrier group does not exist");
		return false;
	}

	if (enemy.ComponentExists<Transform>())
	{
		Transform& transform = enemy.ParentHas<Transform>();
		const glm::vec2& enemyPos = transform.Translation().glm();
		const glm::vec2& enemyScale = transform.Scale().glm();

		for (const EnemySection& section : mBarrierGroups[barrierID].mEnemySections)
		{
			if (enemyPos.x + enemyScale.x / 2 < section.mPos.x - section.mScale.x / 2 ||
				enemyPos.x - enemyScale.x / 2 > section.mPos.x + section.mScale.x / 2 ||
				enemyPos.y + enemyScale.y / 2 < section.mPos.y - section.mScale.y / 2 ||
				enemyPos.y - enemyScale.y / 2 > section.mPos.y + section.mScale.y / 2) // If the missle knows it is not in the section
			{
				continue; // Moves to the next section
			}
			else
			{
				return true; // Is in section, return true
			}
		}
		return false; // Enemy was not in barrier group, return false

	}

	return false; // No transform, can't be in group
}

void FLBarrierManager::RemoveMeFromBarrierGroup(int entityID)
{
	// Note: This whole function ain't being unsubscribed

	Scene* mainScene = FLEngine::Instance().Get<FLSceneManager>()->GetScene("Main Game"); // Gets the main scene

	if (mainScene)
	{
		ECS* mainGameEcs = mainScene->GetECS(); // Gets the ecs of the main scene

		if (mainGameEcs)
		{
			if (mainGameEcs->HasComponent<BehaviourEnemy>(entityID))
			{

				// Finds the enemy ID in the barrier group it's in
				auto enemyPlace = std::find_if(mBarrierGroups[mActiveBarrierGroupID].mEnemies.begin(), mBarrierGroups[mActiveBarrierGroupID].mEnemies.end(),
					[entityID](const EntityID& id) {if (id == static_cast<int>(entityID)) return true; else return false; });

				if (enemyPlace != mBarrierGroups[mActiveBarrierGroupID].mEnemies.end()) // If ID is found
				{
					mBarrierGroups[mActiveBarrierGroupID].mEnemies.erase(enemyPlace); // Removes the enemy ID
				}
			}
		}
	}
}

void FLBarrierManager::ResetActiveBarrierGroup(int placeholder)
{
	if (mActiveBarrierGroupID != static_cast<int>(BarrierNums::NoActiveGroup))
	{
		BarrierGroup& activeGroup = mBarrierGroups[mActiveBarrierGroupID];


		Scene* mainScene = FLEngine::Instance().Get<FLSceneManager>()->GetScene("Main Game"); // Gets the main scene

		if (mainScene)
		{
			ECS* mainGameEcs = mainScene->GetECS(); // Gets the ecs of the main scene

			if (mainGameEcs)
			{
				for (EntityID id : activeGroup.mEnemies) // unsubscribes from the enemy's death events
				{
					mainGameEcs->GetComponentFrommanager<BehaviourEnemy>(id).DeathEvent().Unsubscribe("RemoveEnemy" + std::to_string(id) + "FromBarrierGroup");
				}
				activeGroup.mEnemies.clear(); // Removes all enemies currently tracked



				for (EntityID id : activeGroup.mBarriers) // Resets each barrier
				{
					mainGameEcs->GetComponentFrommanager<BehaviourMapModifier>(id).ResetToReusableState();
					mainGameEcs->GetComponentFrommanager<BehaviourTrapBarrier>(id).ResetToReusable();
				}
			}
		}

		mActiveBarrierGroupID = static_cast<int>(BarrierNums::NoActiveGroup);

		// Audio: Switches to the explore
		FLSound::PostEvent("TO_EXPLORE_BGM");
	}
}
