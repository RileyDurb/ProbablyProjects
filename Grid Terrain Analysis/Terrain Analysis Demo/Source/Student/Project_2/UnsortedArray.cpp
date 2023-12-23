#include <pch.h>
#include "P2_Pathfinding.h"

template<typename T>
UnsortedArray<T>::UnsortedArray()
{
	//mList = new T * [40 * 40];
	mList = NULL; // Don't allocate until allocate function called

	mLastIndex = -1;
}


//template<typename T>
//UnsortedArray<typename T>::UnsortedArray(const UnsortedArray& other)
//{
//	mList = new T * [40 * 40];
//
//	mLastIndex = other.mLastIndex;
//
//	// Deep copies each item
//	for (int i = 0; i <= mLastIndex; i++)
//	{
//		mList[i] = other.mList[i];
//	}
//}

template<typename T>
UnsortedArray<T>::~UnsortedArray()
{
	//int arraySize = 40 * 40;
	//for (int i = 0; i < arraySize; i++)
	//{
	//	mList[i]
	//}
	delete[] mList;
	mList = NULL;
}

template<typename T>
void UnsortedArray<T>::Allocate()
{
	if (mList == NULL)
	{
		mList = new T * [40 * 40];
	}
}


template<typename T>
void UnsortedArray<typename T>::Clear()
{
	// RILEY NOTE: May not need to do this
	// Sets each item to 0
	//for (int i = 0; i <= mLastIndex; i++)
	//{
	//	mList[i] = NULL;
	//}
	
	mLastIndex = -1;
}

template<typename T>
void UnsortedArray<typename T>::PushNode(T* newNode)
{
	mLastIndex++;
	mList[mLastIndex] = newNode;

	//if (mCheapestNodeIndex == -1 || (mList[mCheapestNodeIndex]->totalCost > newNode->totalCost) // if the first, or the cheapest
	//{
	//	mCheapestNodeIndex
	//}
}

template<typename T>
T* UnsortedArray<typename T>::PopCheapest()
{
	//assert(mLastIndex != -1); // If popping before anything was pushed, assert here


	T* cheapestNode = mList[0];
	int cheapestNodeIndex = 0;

	// RILEY NOTE: Maybe don't allocate the int here? We'll see if stuff like that is meaningfull.
	//  UPDATE: yes, it is meaningful, but you acutally just want to allocate it, likely because the cost of allocating and int is much less than the cost to find the one preallocated int

	// Brute force find the cheapest item
	for (int i = 1; i <= mLastIndex; i++) 
	{
		if (mList[i]->totalCost < cheapestNode->totalCost)
		{
			cheapestNode = mList[i];
			cheapestNodeIndex = i;
		}
	}

	mList[cheapestNodeIndex] = mList[mLastIndex]; // Replaces cheapest with the last item

	mLastIndex--; // Decrements last index to decrease array size


	return cheapestNode;
}

template<typename T>
void UnsortedArray<T>::ReplaceNode(T* newNode)
{
	// Brute force find the matching node
	for (int i = 1; i <= mLastIndex; i++)
	{
		if (mList[i]->pos == newNode->pos) // If the matching node
		{
			mList[i] = newNode; // Updates pointer to new node
			return;
		}
	}

	PushNode(newNode);


	return;
}
