#pragma once

// *************************************************
// Simple unsorted array
// 
// Brief: Made for speedily popping the cheapest node, and pushing a new node on
// 
// NOTEs: 
// - Functions assume array is not empty. If you get asserts, you're either doing something wrong with when you call these functions, 
// or I need to understand the usage of this container better.
// - Functions assume the given type has the a GridPos* named pos
// *************************************************

template<typename T>
class UnsortedArray
{
public:
	// Construction
	UnsortedArray();
	//UnsortedArray(const UnsortedArray& other);
	~UnsortedArray();
	void Allocate();

	// Getters and setters
	void PushNode(T* newNode);

	T* PopCheapest();

	// Replaces the node with the same position in this list, or pushes node if there is none
	void ReplaceNode(T* newNode);

	void Clear();

	bool Empty()
	{
		return mLastIndex == -1; // If last index is negative, there are no valid nodes
	}

private:
	// OpNote: Tried making these public so we didn't need to call pushnode, an could just do the operation directly, but that actually made it slower
	T** mList;

	int mLastIndex;

	//int mCheapestNodeIndex = -1;
};


#include "UnsortedArray.cpp"