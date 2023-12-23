#pragma once
/***********************************************************************************************************************
filename    P2_Pathfinding.h
author      Riley Durbin
email       ridurbin0@gmail.com

Brief Description:
  Contains a function, and associated helpers, for performing pathfinding using A*. Optimized to perform well on a speed
  test, so some readbility/ simplicity is sacrificed. Also intended for used with an outside engine, which makes a 
  pathfinding request.

***********************************************************************************************************************/
#include "Misc/PathfindingDetails.hpp"
#include "UnsortedArray.h"
#include "BucketArray.h"

enum class OnList
{
    NoList,
    OpenList,
    ClosedList
};

static OnList nullOnList = OnList::NoList;

struct TestSizeGridNode
{
    TestSizeGridNode* parent = NULL;
    OnList* whichList;
    GridPos pos = GridPos();
    float totalCost = 0;
    float givenCost = 0;
    int validNeighbors = 0;
};

struct GridNode
{
    GridNode(OnList* newOnList); // Default constructor w/ pointer to memory to reference for onlist
    GridNode* parent = NULL;
    GridPos pos = GridPos();
    float totalCost = 0; // A* total cost
    float givenCost = 0; // A* given cost
    OnList& whichList;
    int validNeighbors = 0; // Bitfield of each valid neigbor (8 directions, 8 bits used)
};

class AStarPather
{
public:
    /* 
        The class should be default constructible, so you may need to define a constructor.
        If needed, you can modify the framework where the class is constructed in the
        initialize functions of ProjectTwo and ProjectThree.
    */

    /* ************************************************** */
    // DO NOT MODIFY THESE SIGNATURES
    bool initialize();
    void shutdown();
    PathResult compute_path(PathRequest &request);
    /* ************************************************** */

    void MapChangePrecalcs();

private:

    // void AssignOnLists(); // Assigns the onlist pointers in each node to their spot in the onlist array


    // Post processing ///////////////////////////////////////////////////
    void PostProcess(PathRequest& request, GridNode* parentNode);
    void RubberBandPath(std::vector<GridPos>& processPath);
    void SmoothPath(std::vector<Vec3>& processPath);
    void FillInPath(std::vector<Vec3>& processPath);


    // Debug functions //////////////////////////////////////////
    bool MadeInfiniteParentLoop(GridNode* nodeToCheck);

    bool WillMakeInfiniteLoop(GridNode* nodeToAddTo, GridNode* nodeToAdd);


    // Variables //////////////////////////////////////////////////
    const int MaxMapSize = 40;
    GridNode* mNodeMap[40 * 40]; // Using a normal array, hold pathfinding data for each grid tile
    UnsortedArray<GridNode> mOpenList;

    // NOTE: Used to have this be a 2D array, tried changing it to this for potential cache-friendliness, but performance did not seem to change
    OnList nodeOnLists[40 * 40]; // Outside array of onlists for each node, to allow for memsetting on reset

    // Start and goal of pathing request
    GridPos mStart;
    GridPos mGoal;

    // Heuristic function pointers
    float (*heuristicFuncs[5]) (const GridPos&, const GridPos&);
    float (*currHeuristic) (const GridPos&, const GridPos&);


    // Heuristic scratch variables
    //int xDiff = 0; NOTE: Using scratch variables like this was slowing it down by like 100 us
    //int yDiff = 0;

    // Node usage scratch variables
    //GridNode* parentNode;
    //GridNode* checkNeighbor;
    GridNode scratchNewNeighbor = GridNode(&nullOnList);

    // Neighbor checking scratch variables
    GridPos neighborDirections[8];

};

// NOTE: Made these functions static, so I could put them in an array of function pointers, without using a functor, which gets kind of costly
// Heuristics
float GetOctileDist(const GridPos& from, const GridPos& to);
float GetChebyshevDist(const GridPos& from, const GridPos& to);
float GetEuclideanDist(const GridPos& from, const GridPos& to);
float GetManhattanDist(const GridPos& from, const GridPos& to);
float GetInconsistentDist(const GridPos& from, const GridPos& to);