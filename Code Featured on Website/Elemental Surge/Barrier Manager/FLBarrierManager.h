#pragma once
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
#include "Core.h"
#include "Module.h"
#include "Entity.h"

typedef class BehaviourEnemy;
// Stub Class
class FL_API FLBarrierManager : public Module
{
public:
	void Init();
	void Update();

	void Shutdown();

	void ActivateBarrierGroup(int barrierID);

	void DeactivateBarrierGroup(int barrierID);

	// Adds a barrier to the barrier group
	// Barrier group ID is already set in reading process
	void AddBarrier(Entity barrierObj);

	void AddEnemySection(glm::vec2 position, glm::vec2 scale, int barrierID);

	void RemoveMeFromBarrierGroup(int entityID);

	void ResetActiveBarrierGroup(int placeholder);

	void ClearBarriers();

	static std::string Name() { return "FLBarrierManager"; }
private:
	enum class BarrierNums
	{
		NoActiveGroup = -1
	};

	struct EnemySection
	{
		glm::vec2 mPos;
		glm::vec2 mScale;
	};
	struct BarrierGroup
	{
		std::vector<EntityID> mBarriers; // All barries in group
		std::vector<EnemySection> mEnemySections; // Areas to check for enemies you must defeat to open the barrier group
		std::vector<EntityID> mEnemies; // Enemies found inside enemy sections, which must be defeated
	};

	bool EnemyInBarrierGroup(BehaviourEnemy& enemy, int barrierID);

	std::map<int, BarrierGroup> mBarrierGroups; // The barrier group for each barrier group ID

	int mActiveBarrierGroupID = static_cast<int>(BarrierNums::NoActiveGroup);
};