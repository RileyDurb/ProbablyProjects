#pragma once
//------------------------------------------------------------------------------
//
// File Name:	RandomBin.h
// Author(s):	Riley Durbin
// Class: GAM 150 +
//
// Copyright ? 2023 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "Core.h"

//******************************************************************************//
// Definitions  														        //
//******************************************************************************//

template< typename T>
class  FL_API RandomBin
{
public:
	RandomBin();
	~RandomBin() {};

	// Sets the item's weight
	// If item does not exists, creates a new item with the given weight
	void SetItem(const T& itemName, int weight);

	virtual void AddToItem(const T& itemName, int weight);

	float GetItemWeight(const T& itemName);

	T DrawItem() const; // Picks a random item, bases on set weights

protected:
	std::map<T, int>& GetItems() { return mItems; }
private:
	std::map<T, int> mItems;
};

#include "RandomBin.cpp"

//******************************************************************************//
// Public constants														        //
//******************************************************************************//

//******************************************************************************//
// Public structures													        //
//******************************************************************************//

