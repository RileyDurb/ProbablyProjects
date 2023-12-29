#pragma once
//------------------------------------------------------------------------------
//
// File Name:	FLEvent.h
// Author(s):	Riley Durbin
// Class: GAM 200
//
// Copyright ? 2022 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

// Referenced this linked page for ideas on template metaprogramming for subscribing members:
// https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates#:~:text=A%20implementation%20of%20a%20delegate,compatible%20with%20the%20C%2B%2B%20Standard.

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Core.h"
//#include <functional>
//#include "Script.h"
#include <map>
#include "Module.h"
#include <tuple>
#include "Script.h"
#include <variant>

// Member functions
#include <algorithm>
#include "Behaviour.h"

//typedef class Script Script;

// Used for simplifying subscribing member functions to actions
// type - Name of the class the function is from
// funcName - Name of the member function
// name - Name to be stored as a string
#define SubscribeMember(type, funcName, name) Subscribe<type, &type::funcName>(this, name);


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//


/*
enum class ActionState
{
	Inactive = 0,
	Start = 0,
	During = 1,
	End = 2
};
*/

// Currently not utilized, but give your function one when subscribing
enum class FL_API ActionType
{
	onStart,
	onUpdate,
	onEnd
};

//******************************************************************************//
// Public constants														        //
//******************************************************************************//

//******************************************************************************//
// Public structures													        //
//******************************************************************************//

// Allows std functions to be compared
template <typename ... Args>
bool operator==(const std::function<void(Args...)>& lhs, const std::function<void(Args...)>& rhs)
{
	//return lhs.target_type().hash_code() == rhs.target_type().hash_code();
	return lhs.target<void(Args...)>() == rhs.target<void(Args...)>(); // Compares targets, instead of the actual functions
}


// Use if your event requires parameters
template<typename... Args>
class __declspec(dllexport) Action {
public:
	// Initialization --------------------------------------------------------------------------------------------
	using ActionFunc = std::function<void(Args...)>;
	using ScriptCheckFunc = std::function<bool(void)>;
	using InvokeFunc = std::function<void()>;


	Action(){};

	~Action() = default;
	// Action use  -------------------------------------------------------------------------------------------------
	
	// Calls each subscribed function (given paramaters will be passed to each)

#if 1

	void Invoke(Args... args);

#else 

#endif


	// Adds a function to be called when the event is invoked
	// Use for non-script functions
	Action& Subscribe(const ActionFunc& func, std::string_view name);
	
	// Adds a function to be called when the event is invoked
	// Use for functions inside scripts
	Action& Subscribe(const ScriptContainer<Args...>& func, std::string_view name);

	// Adds a function to be called when the event is invoked
	// Don't call directly, use the SubscribeMember macro. Use for member functions of classes/ structs
	template<class T, void (T::*TMethod)(Args...)>
	Action& Subscribe(T* reciever, std::string_view name);


	// Removes a function from the action
	// Use for non-script functions (both member and non-member)
	Action& Unsubscribe(std::string_view name);


	// Removes a function from the action
	// Use for functions inside scripts
	Action& Unsubscribe(const ScriptContainer<Args...>& funcPkg);

	bool HasActions();

	bool HasCallback(const std::string_view name);

	// Operators /////////////////////////////////////////////////////////////////////////
	
	bool operator==(const Action& rhs);


private:


	// Class to store data for each subscribed function
	class __declspec(dllexport) Callback
	{
	public:


		Callback(const ActionFunc& func, std::string_view name, HINSTANCE* hProc = nullptr);

		Callback();

		template <class T, void (T::* TMethod)(Args...)>
		static Callback from_method(T* object_ptr);

		//Variables /////////////////////////////////////////////////////////////////////////////

		// For member functions 
		typedef void (*memberActionFuncStub)(void* mRecieverObj, Args...);

		std::string mName; // Name used for unsubscribing and comparison
		ActionFunc mFunction; // mFunction to call on event, for non member , non script callbacks
		HINSTANCE* mHProc; // Address of script function comes from (if any)
		void* mRecieverObj; // Member object, of any
		memberActionFuncStub mMFuncPtr; // Packaged member function, if any



		// Operators /////////////////////////////////////////////////////////////////////////

		operator bool();

		void operator()(Args... args) const;

		Callback& operator=(const Callback& rhs)
		{
			this->mName = rhs.mName;
			this->mFunction = rhs.mFunction;
			this->mHProc = rhs.mHProc;
			this->mMFuncPtr = rhs.mMFuncPtr;
			this->mRecieverObj = rhs.mRecieverObj;

			return *this;
		}

		// Helper functions //////////////////////////////////////////////////////////////
		template <class T, void (T::* TMethod)(Args...)> // Class, and member function pointer
		static void memberFuncStub(void* object_ptr, Args... args); // Class pointer MFP is in, args for call
	};

	// Helper functions ////////////////////////////////////////////////
	Callback* GetCallback(const std::string_view name)
	{
		for (Callback& callback : mCallbackList)
		{
			if (callback.mName == name) // If callback with given function is found
			{
				return &callback;
			}
		}

		return NULL; // Callback with given function was not found
	}


	// Variables ///////////////////////////////////////////////////////
	std::vector<Callback>mCallbackList; // Holds all subscribed functions ... yet the majority are still unsubscribed ):
};

#include "FLEvent.cpp"