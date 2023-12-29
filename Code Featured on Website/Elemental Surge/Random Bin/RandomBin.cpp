//------------------------------------------------------------------------------
//
// File Name:	RandomBin.cpp
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
#include "RandomBin.h"
#include "Random.h"


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

template<typename T>
RandomBin<T>::RandomBin()
{
}


// Sets the item's weight
// If item does not exists, creates a new item with the given weight
template<typename T>
void RandomBin<T>::SetItem(const T& itemName, int weight)
{
	mItems[itemName] = weight;
}

template<typename T>
void RandomBin<T>::AddToItem(const T& itemName, int weight)
{
	mItems[itemName] += weight;

	if (mItems[itemName] <= 0)
	{
		mItems[itemName] = 0;
	}
}

template<typename T>
float RandomBin<T>::GetItemWeight(const T& itemName)
{
	if (mItems.contains(itemName))
	{
		return mItems[itemName];
	}
	else
	{
		return 0;
	}
}

template<typename T>
T RandomBin<T>::DrawItem() const
{
	if (mItems.empty())
	{
		return T();
	}

	// Calculates the total weight
	int weightTotal = 0;
	for (auto it = mItems.begin(); it != mItems.end(); it++)
	{
		weightTotal += it._Ptr->_Myval.second;
	}

	// Gets a random value for deciding which item to choose
	int chosenValue = Random::RangeInt(0, weightTotal - 1);

	// Finds which item the chosen value falls under, and returns chosen item
	int lastRangeEnd = 0;
	for (auto it = mItems.begin(); it != mItems.end(); it++) // For each item
	{
		if (chosenValue >= lastRangeEnd && chosenValue < lastRangeEnd + it._Ptr->_Myval.second) // If in this item's random range
		{
			return it._Ptr->_Myval.first; // Returns chosen item
		}

		lastRangeEnd += it._Ptr->_Myval.second; // Moves past this range
	}

	return T(); // if this reaches here, one of the weights is likely negative
}
