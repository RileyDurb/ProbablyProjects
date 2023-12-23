#include "e-dijkstra.h"
/***********************************************************************************************************************
filename    e-dijkstra.cpp
author      Riley Durbin
email       ridurbin0@gmail.com

Brief Description:
  Contains functions for solving the electric dijkstra problem, where given a connected, undirected graph, we see if 
  an electric car, starting on empty, with K maximum recharges, and R range, can make it to each city from any city.

***********************************************************************************************************************/
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <limits>

struct CarState
{
    // Constructors
    CarState(float batteryLeft, float rechargesLeft)
        : mBatteryLeft(batteryLeft)
        , mRechargesLeft(rechargesLeft)
    {

    }

    CarState()
        : mBatteryLeft(0.0f)
        , mRechargesLeft(0.0f)
    {

    }
    // Operators
    bool operator<(const CarState& other) const
    {
        // First compare recharges left
        if (other.mRechargesLeft > mRechargesLeft)
        {
            return true;
        }

        if (other.mRechargesLeft < mRechargesLeft)
        {
            return false;
        }

        if (other.mBatteryLeft > mBatteryLeft)
        {
            return true;
        }
        else
        {
            return false; // either greater than, or equal to, return false
        }
    }

    bool operator==(const CarState& other)
    {
        if (mBatteryLeft == other.mBatteryLeft &&
            mRechargesLeft == other.mRechargesLeft)
        {
            return true;
        }

        return false;
    }

    float mBatteryLeft = 0;
    float mRechargesLeft = 0;
    
};
struct MapEdge
{
    // Constructors
    MapEdge(const CarState& startState, const int locationIndex)
        : state(startState)
        , index(locationIndex)
    {

    }

    bool operator<(const MapEdge& other) const
    {
        return state < other.state;
    }

    CarState state = CarState();
    int index = 0;
    bool evaluated = false;
};


class UpdateQueue
{
public:

    UpdateQueue()
        : mList()
    {

    }

    // Returns true if location existed
    bool Update(int locationIndex, CarState& newState)
    {
        for (int i = 0; i < static_cast<int>(mList.size()); i++)
        {
            if (mList[i].index == locationIndex)
            {
                mList[i].state = newState;
                std::make_heap(mList.begin(), mList.end());
                return true;
            }
        }

        return false;
    }

    void Update(MapEdge* currEdge, CarState& newState)
    {
        currEdge->state = newState;
        std::make_heap(mList.begin(), mList.end());
    }

    MapEdge* Get(int locationIndex)
    {
        for (int i = 0; i < static_cast<int>(mList.size()); i++)
        {
            if (mList[i].index == locationIndex)
            {
                return &mList[i];
            }
        }

        std::cout << "Invalid Index";
        throw "Invalid Index";
        return NULL;

    }

    void Push(MapEdge newEdge)
    {
        mList.push_back(newEdge);
        std::make_heap(mList.begin(), mList.end()); // Sorts using default of less than
    }

    MapEdge Top()
    {
        return mList.front();
    }

    void Pop()
    {
        mList.erase(mList.begin());
    }


    // Query functions

    bool Empty()
    {
        return mList.empty();
    }
private:
    std::vector<MapEdge> mList;
};
// Helper functions ///////////////////////////////////////////////////////////////////
std::vector<std::vector<int>> ReadMap(char const* filename, int & numLocations, int& numCharges, int& numEdges)
{
	std::vector<std::vector<int>> cityMap;

    std::ifstream in(filename, std::ifstream::in);


    if (!in.is_open()) {
        std::cout << "problem reading " << filename << std::endl;
        return std::vector<std::vector<int>>();
    }

    // Reads in starting info
    in >> numLocations;
    in >> numCharges;
    in >> numEdges;

    // inits all values to maxs
    for (int i = 0; i < numLocations; ++i) {
        std::vector<int> row;
        for (int j = 0; j < numLocations; ++j) {
            row.push_back(std::numeric_limits<int>::max());
        }
        cityMap.push_back(row);
    }
    // Reads in
    int location1 = 0;
    int location2 = 0;
    for (int i = 0; i < numEdges; ++i) {
        if (!in.good()) {
            std::cout << "problem reading " << filename << std::endl;
            return std::vector<std::vector<int>>();
        }

        // Reads in locations
        in >> location1;
        in >> location2;

        // Reads distance between locations, and sets in map
        in >> cityMap[location1][location2];
        cityMap[location2][location1] = cityMap[location1][location2];
    }

    return cityMap;
}


bool e_dijkstra_all(int startCity, int range, int numCharges, int numLocations, std::vector<std::vector<int>>& cityDistanceMap, std::vector<MapEdge>& cityList)
{
    std::map<int, bool> reachableLocations; // For keeping track of which verticies we/ve found a path to

    // Reset all edges
    for (int i = 0; i < numLocations; i++)
    {

        // Resets state
        cityList[i].state.mBatteryLeft = 0;
        cityList[i].state.mRechargesLeft = 0;

        // Sets index
        cityList[i].index = i;

        // Resets evaluated
        cityList[i].evaluated = false;
    }
    cityList[startCity].state.mBatteryLeft = 0; // Sets starting battery to 0
    cityList[startCity].state.mRechargesLeft = static_cast<float>(numCharges); // Sets starting battery to 0

    CarState currCarState; // For tracking our current car state

    // Push all nodes into the list
    UpdateQueue openList;

    openList.Push(MapEdge({ 0, static_cast<float>(numCharges) }, startCity));

    while (openList.Empty() == false)
    {
        // Get next best node
        MapEdge currCity = openList.Top();
        openList.Pop();

        reachableLocations.emplace(currCity.index, true); // Add the new city to the tracker for found locations

        cityList[currCity.index].evaluated = true; // Set current city as evaluated

        currCarState = currCity.state; // Advances car state to the state at the current city

        // evaluate all neighbors for our current neighbor
        for (int k = 0; k < numLocations; k++)
        {
            MapEdge& checkCity = cityList[k];

            if (cityDistanceMap[currCity.index][k] >= (2147483647 - 5)) // If distance is around infinity
            {
                continue; // Not a neighbor, skip
            }

            if (checkCity.evaluated == true)
            {
                continue; // Skiiiip
            }

            // try moving to current neighbor
            CarState alt = currCarState;
            int distanceToNeighbor = cityDistanceMap[currCity.index][k];
            alt.mBatteryLeft -= distanceToNeighbor;
            if (alt.mBatteryLeft < 0) // if not enough charge to get to neighbor
            {
                if (range < distanceToNeighbor) // If out of range
                {
                    continue; // Impossible to get there from here, skip
                }

                if (alt.mRechargesLeft > 0) // if We have charges left
                {
                    alt.mRechargesLeft--; // Uses a recharge
                    alt.mBatteryLeft = range - distanceToNeighbor; // Travels distance, subtracting from new full charge
                }
                else
                {
                    continue; // no charge, we're done. continue
                }
            }

            if (checkCity.state < alt || (checkCity.state.mBatteryLeft == 0 && checkCity.state.mRechargesLeft == 0))// This path leaves us in a better state than what was previously saved, or there was no previous state written
            {
                checkCity.state = alt; // Saved new state as best
                bool locationAlreadyExists = openList.Update(checkCity.index, checkCity.state); // tries to update if it can

                if (locationAlreadyExists == false) // if there was no node to update
                {
                    openList.Push(checkCity); // Push that son
                }
            }
        }
    }

    if (static_cast<int>(reachableLocations.size()) == numLocations)
    {
        return true;
    }
    else
    {
        return false;
    }

}


// Notes for dijkstra
/* 
- Don't need to keep track of evaluated, doesn't give a noticable optimization

*/

// ////////////////////////////////////////////////////////////////////////////////////
bool e_dijkstra(char const* filename, int range)
{
    int numLocations;
    int numCharges;
    int numEdges;
    std::vector<std::vector<int>> cityDistanceMap = ReadMap(filename, numLocations, numCharges, numEdges);
    
    std::vector<MapEdge> cityList = std::vector<MapEdge>(numLocations, MapEdge({ 0,0 }, 0)); // Makes a list of edges, one for each city, for storing the state of the car when reaching them (the given cost)
    // Priority queue version -------------------------------------------------------------------------------
    for (int i = 0; i < numLocations - 1; i++)
    {
		bool allPathsFound = e_dijkstra_all(i, range, numCharges, numLocations, cityDistanceMap, cityList);
		if (allPathsFound == false) // If no path
		{
			return false; // One path no found is all we need, return false
		}
    }

    return true; // All paths found

    
    /*
    High level:
    for each index, except the last, can it match with the rest (starting from i + 1)

    */

}
