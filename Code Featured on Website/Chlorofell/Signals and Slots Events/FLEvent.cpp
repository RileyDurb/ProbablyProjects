//------------------------------------------------------------------------------
//
// File Name:	FLEvent.cpp
// Author(s):	Riley Durbin
// Class: GAM 200
//
// Copyright ? 2022 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include <functional>
#include "FLEvent.h"
#include "FLInput.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

// Action ------------------------------------------------------------------------
// Public Functions /////////////////////////////////////////////////////////////////

template<typename ... Args>
void Action<Args...>::Invoke(Args... args)
{
	if (mCallbackList.size()) // If there are an actions
	{
		// Loops through each callback
		for (int i = 0; i < mCallbackList.size(); ++i)
		{
			Callback& funcPkg = mCallbackList[i];
			if (funcPkg && (funcPkg.mHProc == nullptr || *funcPkg.mHProc)) // If not a script function, or HProc of script is valid
			{
				funcPkg(args...); // Calls the subscribed function
			}
			else // Invalid script function
			{
				mCallbackList.erase(mCallbackList.begin() + i); // Removes its callback from the action
			}
		}
	}
}

//template<typename ... Args>
//void Action<Args...>::Start(Args... args)
//{
//	Reset(); // Readies all callbacks for new calling
//
//	mArguments = std::forward_as_tuple((args)...); // Packs arguments into a tuple
//	//auto& arg = std::get<0>(mArguments); // Makes a direct reference out of the tuple
//	//mInvokeFunc = [arg, this]() { Invoke(arg); }; // Packages arguments with the function, for use while updating
//
//	isActive = true;
//}

template<typename ... Args>
bool Action<Args...>::HasActions()
{
	if (mCallbackList.size())
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<typename ...Args>
bool Action<Args...>::HasCallback(const std::string_view name)
{
	if (GetCallback(name))
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<typename ... Args>
Action<Args...>& Action<Args...>::Subscribe(const ActionFunc& func, std::string_view name)
{
	mCallbackList.emplace_back(Callback(func, name));

	//if constexpr (std::is_void_v<Args...>)
	//{
	//	printf("voidboi");
	//}


	return *this;  // Returns reference to itself
}

template<typename ... Args>
Action<Args...>& Action<Args...>::Subscribe(const ScriptContainer<Args...>& func, std::string_view name)
{
	mCallbackList.emplace_back(Callback(func.mFunc, name, func.mHProc));

	return *this;  // Returns reference to itself
}


template<typename ... Args>
template<class T, void (T::* TMethod)(Args...)>
Action<Args...>& Action<Args...>::Subscribe(T* reciever, std::string_view name)
{
	// Makes a callback with a function pointer usable outside of the reciver class
	Callback newCallback = Callback(Callback::from_method<T, TMethod>(reciever));

	newCallback.mName = name; // Adds name, rest is default or assgned with from_method

	mCallbackList.emplace_back(newCallback);

	return *this;  // Returns reference to itself
}


template<typename ... Args>
Action<Args...>& Action<Args...>::Unsubscribe(std::string_view name)
{
	int i = 0;
	for (Callback& checkCallback : mCallbackList)
	{
		if (checkCallback.mName == name)
		{
			mCallbackList.erase(mCallbackList.begin() + i);
			break;
		}
		i++;
	}

	return *this; // Returns reference to itself
}

template<typename ... Args>
Action<Args...>& Action<Args...>::Unsubscribe(const ScriptContainer<Args...>& funcPkg)
{
	int i = 0;
	for (Callback& checkCallback : mCallbackList)
	{
		if (checkCallback.mFunction == funcPkg.mFunc) // Checks if function matches given one
		{
			mCallbackList.erase(mCallbackList.begin() + i);
			break;
		}
		i++;
	}

	return *this; // Returns reference to itself
}

template<typename ... Args>
bool Action<Args...>::operator==(const Action<Args...>& rhs)
{
	if (mCallbackList.size() != rhs.mCallbackList.size()) // Compares list sizes
	{
		return false;
	}
	
	for (int i = 0; i < mCallbackList.size(); ++i) // Compares the names of each callback
	{
		if (mCallbackList.at(i).mName != rhs.mCallbackList.at(i).mName)
		{
			return false;
		}
	}

	return true;
}


// Private Functions /////////////////////////////////////////////////////////////////
//template<typename ... Args>
//Action<Args...>::Callback* Action<Args...>::GetCallback(const ActionFunc& func)
//{
//	for (Callback& callback : mCallbackList)
//	{
//		if (callback.function == func) // If callback with given function is found
//		{
//			return &callback;
//		}
//	}
//
//	return nullptr; // Callback with given function was not found
//}

// Callback -------------------------------------------------------------------------
template<typename ... Args>
Action<Args...>::Callback::Callback(const ActionFunc& func, std::string_view name, HINSTANCE* hProc)
	: mName(name)
, mFunction(func)
, mHProc(hProc)
, mRecieverObj(NULL)
, mMFuncPtr(NULL)
{
}

template<typename ... Args>
Action<Args...>::Callback::Callback()
:	mName()
, mFunction(NULL)
, mHProc(nullptr)
, mRecieverObj(NULL)
, mMFuncPtr(NULL)
{

}

// Operators //////////////////////////////////////////////////////////////////
template<typename ... Args>
Action<Args...>::Callback::operator bool()
{
	return true;
}

template<typename ... Args>
template <class T, void (T::* TMethod)(Args...)>
Action<Args...>::Callback Action<Args...>::Callback::from_method(T* object_ptr)
{
	Callback funcPkg;
	funcPkg.mRecieverObj = object_ptr;
	funcPkg.mMFuncPtr = &memberFuncStub<T, TMethod>; // Turns method into a usable pointer
	return funcPkg;
}

template<typename ... Args>
void Action<Args...>::Callback::operator()(Args... args) const
{
	// Calls regular function, or member version if it has that instead
	if (mFunction)
	{
		mFunction(args...);
	}
	else
	{
		(*mMFuncPtr)(mRecieverObj, args...);
	}
}
//
//template<typename ... Args>
//Action<Args...>::Callback& Action<Args...>::Callback::operator=(const Callback& rhs)
//{
//	this->mName = rhs.mName;
//	this->mFunction = rhs.mFunction;
//	this->mHProc = rhs.mHProc;
//	this->mMFuncPtr = rhs.mMFuncPtr;
//	this->mRecieverObj = rhs.mRecieverObj;
//
//	return *this;
//}

template<typename ... Args>
template <class T, void (T::* TMethod)(Args...)> // Class, and member function pointer
void Action<Args...>::Callback::memberFuncStub(void* object_ptr, Args... args)
{
	T* p = static_cast<T*>(object_ptr); // cast object to its type
	return (p->*TMethod)(args...); // call
}