//------------------------------------------------------------------------------
//
// File Name:	Button.cpp
// Author(s):	Riley Durbin
// Class: GAM 200
//
// Copyright ? 2022 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "Component.h"
#include "Button.h"
#include "JsonHelper.h"
#include "Tag.h" // On collide with cursor
#include "FLEventList.h"
#include "Collider.h" // On collide function
#include "ButtonFunction.h"
#include "Sprite.h"

/*
NOTE for refactoring:
The three similar versions of functions for hold, hover, and release, could likely be re-written as one if
I made a class that housed the functionality, and just made an instance for each one. May not be helpful
as all needed button functionality is made and tested, but good to keep in mind.

*/

Button::Button(EntityID parent)
	: Component(parent) // Type can be set only during construction.
	, mOGScale()
	, mIsColliding(false)
{
	mOGScale.Set(0, 0);
	mButtonStates.emplace("Hold", ActiveState::mNoContact);
	mButtonStates.emplace("Hover", ActiveState::mNoContact);
	mButtonStates.emplace("Release", ActiveState::mNoContact);
}


Button::Button(const Button& other) : Component(other)
{
	*this = other;
}

bool Button::operator==(Button const& other) const
{
	if (this->mOnHoldFuncNames == other.mOnHoldFuncNames &&
		this->mOnHoverFuncNames == other.mOnHoverFuncNames &&
		this->mOnReleaseFuncNames == other.mOnReleaseFuncNames) // Compares names
	{
		return true;
	}
	else
	{
		return false;
	}
}


Button::~Button()
{
}

// Create a clone of the current component
Button& Button::Clone() const
{
	Button* newButton = new Button(*this);

	return *newButton;
}


void Button::Read(rapidjson::Value& component)
{
	if (component.HasMember("OnHover"))
	{
		std::string funcNames = GetMemberValue(component, "OnHover").GetString();

		// Clears names vector, so no duplicates carry over
		mOnHoverFuncNames.clear();
		// Saves as separate function names
		JsonHelper::StringToStringList(mOnHoverFuncNames, funcNames);

		// Subscribes each function
		for (std::string funcName : mOnHoverFuncNames)
		{			
			
			ButtonFunction::ButtonActionFunc hoverFunc = ButtonFunction::GetFunc(funcName);
			if (hoverFunc && !mOnHover.HasCallback(funcName)) // If invalid, function name read was invalid. Also, no duplicates
				mOnHover.Subscribe(hoverFunc, funcName);
		}
	}
	if (component.HasMember("OnHold"))
	{
		std::string funcNames = GetMemberValue(component, "OnHold").GetString();

		// Clears names vector, so no duplicates carry over
		mOnHoldFuncNames.clear();

		// Saves as separate function names
		JsonHelper::StringToStringList(mOnHoldFuncNames, funcNames);

		// Subscribes each function
		for (std::string funcName : mOnHoldFuncNames)
		{
			// Doesn't subscribe a duplicate
			
			ButtonFunction::ButtonActionFunc holdFunc = ButtonFunction::GetFunc(funcName);
			if (holdFunc && !mOnHold.HasCallback(funcName)) // If invalid, function name read was invalid. Also, no duplicates
				mOnHold.Subscribe(holdFunc, funcName);
		}
	}
	if (component.HasMember("OnRelease"))
	{
		std::string funcNames = GetMemberValue(component, "OnRelease").GetString();

		// Clears names vector, so no duplicates carry over
		mOnReleaseFuncNames.clear();
		 
		// Saves as separate function names
		JsonHelper::StringToStringList(mOnReleaseFuncNames, funcNames);

		// Subscribes each function
		for (std::string funcName : mOnReleaseFuncNames)

		{
			
			ButtonFunction::ButtonActionFunc releaseFunc = ButtonFunction::GetFunc(funcName);
			if (releaseFunc && !mOnRelease.HasCallback(funcName)) // If invalid, function name read was invalid. Also, no duplicates
				mOnRelease.Subscribe(releaseFunc, funcName);
		}
	}
}

void Button::Write(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Button& butt = *this;

	// Writes type
	writer.StartObject();
	writer.Key("type");
	writer.String(butt.Name().data());

	// If any, writes a string of the names of its on hover functions
	if (!mOnHoverFuncNames.empty())
	{
		writer.Key("OnHover");
		std::string funcNames;
		JsonHelper::StringListToString(funcNames, mOnHoverFuncNames);
		writer.String(funcNames.c_str());
	}
	// If any, writes a string of the names of its on hold functions
	if (!mOnHoldFuncNames.empty())
	{
		writer.Key("OnHold");
		std::string funcNames;
		JsonHelper::StringListToString(funcNames, mOnHoldFuncNames);
		writer.String(funcNames.c_str());
	}
	// If any, writes a string of the names of its on release functions
	if (!mOnReleaseFuncNames.empty())
	{
		writer.Key("OnRelease");
		std::string funcNames;
		JsonHelper::StringListToString(funcNames, mOnReleaseFuncNames);
		writer.String(funcNames.c_str());
	}

	writer.EndObject();

}

void Button::Init()
{
	if (ComponentExists<Collider>())
	{
		ParentHas<Collider>().CollisionEvent().SubscribeMember(Button, OnCollide, "Collider");
	}
	if (ComponentExists<Transform>())
	{
		Transform& transform = ParentHas<Transform>();
		mOGScale = transform.Scale();

		//mOGPos = transform.Translation();
	}
}

void Button::Update(float dt)
{
	//Reset();
	UndoModActions();
	UpdateStates();

	// RILEY NOTE: Debug here to trigger
	//if (Input::KeyPressed(GLFW_KEY_F10))
	//{
	//	ButtonFunction::GetFunc("SwitchToSettingsMenu")(NULL, *this);
	//}
}

void Button::Shutdown()
{
	// Unsubscribes each function
	if (ComponentExists<Collider>())
	{
		ParentHas<Collider>().CollisionEvent().Unsubscribe("Collider");
	}
	for (std::string funcName : mOnHoverFuncNames)
	{
		ButtonFunction::ButtonActionFunc hoverFunc = ButtonFunction::GetFunc(funcName);
		if (hoverFunc) // If invalid, function name read was invalid
			mOnHover.Unsubscribe(funcName);
	}
	for (std::string funcName : mOnHoldFuncNames)
	{
		ButtonFunction::ButtonActionFunc holdFunc = ButtonFunction::GetFunc(funcName);
		if (holdFunc) // If invalid, function name read was invalid
			mOnHold.Unsubscribe(funcName);
	}
	for (std::string funcName : mOnReleaseFuncNames)
	{
		ButtonFunction::ButtonActionFunc releaseFunc = ButtonFunction::GetFunc(funcName);
		if (releaseFunc) // If invalid, function name read was invalid
			mOnRelease.Unsubscribe(funcName);
	}
}

void Button::Reset()
{
	if (ComponentExists<Transform>())
	{
		Transform& transform = ParentHas<Transform>();
		transform.Scale() = mOGScale;
	}
	if (ComponentExists<Sprite>())
	{
		Sprite& sprite = ParentHas<Sprite>();
		sprite.SetTint(glm::vec4(0, 0, 0, 0));
	}
}

void Button::OnCollide(ColliderPtr obj2)
{
	Component& comp = this->Manager()->Get(this->ID());
	if (obj2)
	{
		if (ComponentExists<Collider>())
		{
			if (obj2->ComponentExists<Tag>())
			{
				Tag& other = obj2->ParentHas<Tag>();

				if (other.ETag() == "Cursor")
				{
					// On click
					if (Input::MouseReleased(GLFW_MOUSE_BUTTON_LEFT))
					{
						mOnRelease.Invoke(obj2, comp);
						mButtonStates["Release"] = ActiveState::mColliding;
					}
					else if (Input::MousePressed(GLFW_MOUSE_BUTTON_LEFT)) // On release
					{
						mOnHold.Invoke(obj2, comp);
						mButtonStates["Hold"] = ActiveState::mColliding;

					}
					else // On hover
					{
						mOnHover.Invoke(obj2, comp);
						mButtonStates["Hover"] = ActiveState::mColliding;
					}

				}
			}
		}
	}
 }

std::string Button::GetOnHoverFuncNames()
{
	std::string outStr = "";
	JsonHelper::StringListToString(outStr, mOnHoverFuncNames); // turns on hover func names into a single string

	return outStr;
}

std::string Button::GetOnHoldFuncNames()
{
	std::string outStr = "";
	JsonHelper::StringListToString(outStr, mOnHoldFuncNames); // turns on hold func names into a single string

	return outStr;
}

std::string Button::GetOnReleaseFuncNames()
{
	std::string outStr = "";
	JsonHelper::StringListToString(outStr, mOnReleaseFuncNames); // turns on release func names into a single string

	return outStr;
}

void Button::SetOnHoverFuncsByName(std::string funcNames)
{
	// unsubscibes current functions
	for (std::string funcName : mOnHoverFuncNames)
	{
		ButtonFunction::ButtonActionFunc hoverFunc = ButtonFunction::GetFunc(funcName);
		if (hoverFunc) // If invalid, function name read was invalid
			mOnHover.Unsubscribe(funcName);
	}

	// Converts and sets new func names
	mOnHoverFuncNames.clear();
	JsonHelper::StringToStringList(mOnHoverFuncNames, funcNames);

	// Subscribes new functions
	for (std::string funcName : mOnHoverFuncNames)
	{
		ButtonFunction::ButtonActionFunc hoverFunc = ButtonFunction::GetFunc(funcName);
		if (hoverFunc && !mOnHover.HasCallback(funcName)) // If invalid, function name read was invalid. Also, duplicates
			mOnHover.Subscribe(hoverFunc, funcName);
	}
	
}

void Button::SetOnHoldFuncsByName(std::string funcNames)
{
	// unsubscibes current functions

	for (std::string funcName : mOnHoldFuncNames)
	{
		ButtonFunction::ButtonActionFunc holdFunc = ButtonFunction::GetFunc(funcName);
		if (holdFunc) // If invalid, function name read was invalid
			mOnHold.Unsubscribe(funcName);
	}

	// Converts and sets new func names
	mOnHoldFuncNames.clear();
	JsonHelper::StringToStringList(mOnHoldFuncNames, funcNames);

	// Subscribes new functions
	for (std::string funcName : mOnHoldFuncNames)
	{
		// Doesn't subscribe a duplicate

		ButtonFunction::ButtonActionFunc holdFunc = ButtonFunction::GetFunc(funcName);
		if (holdFunc && !mOnHold.HasCallback(funcName)) // If invalid, function name read was invalid. Also, duplicates
			mOnHold.Subscribe(holdFunc, funcName);
	}
}

void Button::SetOnReleaseFuncsByName(std::string funcNames)
{
	// unsubscibes current functions
	for (std::string funcName : mOnReleaseFuncNames)
	{
		ButtonFunction::ButtonActionFunc releaseFunc = ButtonFunction::GetFunc(funcName);
		if (releaseFunc) // If invalid, function name read was invalid
			mOnRelease.Unsubscribe(funcName);
	}
	
	// Converts and sets new func names
	mOnReleaseFuncNames.clear();
	JsonHelper::StringToStringList(mOnReleaseFuncNames, funcNames);

	// Subscribes new functions
	for (std::string funcName : mOnReleaseFuncNames)
	{

		ButtonFunction::ButtonActionFunc releaseFunc = ButtonFunction::GetFunc(funcName);
		if (releaseFunc && !mOnRelease.HasCallback(funcName)) // If invalid, function name read was invalid. Also, duplicates
			mOnRelease.Subscribe(releaseFunc, funcName);
	}
}

void Button::AddModUndo(ModUndo action, std::string_view name)
{
	mUndoModActions.push_back(ModUndoPair(action, name));
}

void Button::PlaySFXOnButton(std::string_view eventName, std::string_view stateType, AkGameObjectID GOID)
{
	if (mButtonStates[stateType.data()] == ActiveState::mNoContact)
	{
		AK::SoundEngine::PostEvent(eventName.data(), GOID);
	}
}

//bool Button::IsFirstRelease()
//{
//	return
//}

void Button::UndoModActions()
{
	// Undos each action
	for (int i = mUndoModActions.size() - 1; i >= 0; i--) // Reverse order, so that the calls reverse things to the beginning
	{
		ModUndoPair undo = mUndoModActions[i];
		undo.first();
	}

	// Removes all actions
	mUndoModActions.clear();
}

void Button::UpdateStates()
{
	UpdateState("Hover");
	UpdateState("Hold");
	UpdateState("Release");
}

void Button::UpdateState(std::string_view category)
{
	if (mButtonStates[category.data()] == ActiveState::mColliding) // If currently colliding
	{
		mButtonStates[category.data()] = ActiveState::mEndingBuffer; // Sets button to be reset next frame
	}
	else if (mButtonStates[category.data()] == ActiveState::mEndingBuffer) // If Collision has not been continued, or no functions needing it's collision state are used
	{
		mButtonStates[category.data()] = ActiveState::mNoContact; // 
	}
}


