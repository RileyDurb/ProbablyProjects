//------------------------------------------------------------------------------
//
// File Name:	Button.h
// Author(s):	Riley Durbin
// Class: GAM 200
//
// Copyright ? 2022 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------
#pragma once
#include "stdafx.h"
#include "Core.h"
#include "Component.h"
#include "FLEvent.h"
#include "FLSound.h"


/*
NOTE for refactoring:
The three similar versions of functions for hold, hover, and release, could likely be re-written as one if
I made a class that housed the functionality, and just made an instance for each one. May not be helpful
as all needed button functionality is made and tested, but good to keep in mind.

*/

typedef class FL_API Button* ButtonPtr;


class FL_API Button : public Component
{
public:
	enum class ActiveState {
		mNoContact,
		mColliding,
		mEndingBuffer,
	};
	using ModUndo = std::function<void()>;
	using ModUndoPair = std::pair<ModUndo, std::string>;

	Button(EntityID parent);

	Button(const Button& other);

	~Button();

	Button& Clone() const;

	void Read(rapidjson::Value& stream);

	void Write(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	void Init();
	
	void Update(float dt);

	void Shutdown();

	void Reset();

	static std::string_view TypeName() { return "Button"; }

	bool operator==(Button const& other) const;

	// Event Functions
	void OnCollide(ColliderPtr obj2);

	// Getters and setters

	// returns a string of the on hover functions
	std::string GetOnHoverFuncNames();
	std::string GetOnHoldFuncNames();
	std::string GetOnReleaseFuncNames();

	const std::vector<std::string>& GetOnHoverFuncNamesVec() { return mOnHoverFuncNames; }
	const std::vector<std::string>& GetOnHoldFuncNamesVec() { return mOnHoldFuncNames; }
	const std::vector<std::string>& GetOnReleaseFuncNamesVec() { return mOnReleaseFuncNames; }

	// takes in a formatted list of function names, and sets to button
	void SetOnHoverFuncsByName(std::string funcNames);
	void SetOnHoldFuncsByName(std::string funcNames);
	void SetOnReleaseFuncsByName(std::string funcNames);

	// Undoing mods to the button
	void AddModUndo(ModUndo action, std::string_view name);

	// SFX
	void PlaySFXOnButton(std::string_view eventName, std::string_view stateType, AkGameObjectID GOID = FLSound::UniversalGOID());


	// Action state
	//bool IsFirstRelease();

	// ECS functions
	EntityID ID() { return Component::ID(); };

	IComponentManagerPtr Manager() { return Component::Manager(); }

private:
	// Helper Functions

	void UndoModActions();

	void UpdateStates(); // Updates all states

	void UpdateState(std::string_view category); // Updates the button state of the given string's category

	std::string_view Name() const override { return TypeName(); }

	std::vector<std::string> mOnHoverFuncNames;
	std::vector<std::string> mOnHoldFuncNames;
	std::vector<std::string> mOnReleaseFuncNames;
	Action<ColliderPtr, Component&> mOnHover;
	Action<ColliderPtr, Component&> mOnHold;
	Action<ColliderPtr, Component&> mOnRelease;

	std::vector<ModUndoPair> mUndoModActions;

	bool mIsColliding;


	std::map<std::string, ActiveState> mButtonStates;

	// Original Data
	vector2D mOGScale;
	//vector2D mOGPos;
};

