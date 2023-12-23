/***********************************************************************************************************************
filename    P2_Pathfinding.cpp
author      Riley Durbin
email       ridurbin0@gmail.com

Brief Description:
  Contains a function, and associated helpers, for performing pathfinding using A*. NOTE: Optimized to perform well on
  a speed test, so some readbility/ simplicity is sacrificed. Also intended for used with an outside engine, which
  makes a pathfinding request.

***********************************************************************************************************************/

#include <pch.h>
#include "Projects/ProjectTwo.h"
#include "P2_Pathfinding.h"

const static float sqrt2f = (float)M_SQRT2;

const static float diagonalDistanceBonus = sqrt2f - 1;

#pragma region Extra Credit
bool ProjectTwo::implemented_floyd_warshall()
{
    return false;
}

bool ProjectTwo::implemented_goal_bounding()
{
    return false;
}

bool ProjectTwo::implemented_jps_plus()
{
    return false;
}
#pragma endregion

bool AStarPather::initialize()
{
    // handle any one-time setup requirements you have

    int onListPos = 0;
    // Allocates the WHOLE list
    for (int y = 0; y < MaxMapSize; y++)
    {
        for (int x = 0; x < MaxMapSize; x++)
        {
            GridNode* currNode = mNodeMap[y * MaxMapSize + x] = new GridNode(& nodeOnLists[onListPos]);
            // Assigns position
            currNode->pos.col = x;
            currNode->pos.row = y;

            onListPos++;
            //(*currNode->whichList) = OnList::NoList; // NOTE: If I am doing things the list way, can't assign on list here, cause pointer hasn't been set yet
        }
    }

    memset(nodeOnLists, 0, sizeof(OnList) * MaxMapSize * MaxMapSize); // Resets the onlist array (don't technically need this, cause I do it at the beginning of the search)

    mOpenList.Allocate(); // Allocates here, to hopefully put the list near the cache of nodes

    // Sets up heuristic function pointers
    heuristicFuncs[static_cast<int>(Heuristic::OCTILE)] = &GetOctileDist;
    heuristicFuncs[static_cast<int>(Heuristic::CHEBYSHEV)] = &GetChebyshevDist;
    heuristicFuncs[static_cast<int>(Heuristic::MANHATTAN)] = &GetManhattanDist;
    heuristicFuncs[static_cast<int>(Heuristic::EUCLIDEAN)] = &GetEuclideanDist;
    heuristicFuncs[static_cast<int>(Heuristic::INCONSISTENT)] = &GetInconsistentDist;

    // Sets up neighbor directions from N, clockwise all the way to NW
    GridPos direction;
    // N
    direction.col = 0;
    direction.row = 1;
    neighborDirections[0] = direction;
    // NE
    direction.col = 1;
    direction.row = 1;
    neighborDirections[1] = direction;
    // E
    direction.col = 1;
    direction.row = 0;
    neighborDirections[2] = direction;
    // SE
    direction.col = 1;
    direction.row = -1;
    neighborDirections[3] = direction;
    // S
    direction.col = 0;
    direction.row = -1;
    neighborDirections[4] = direction;
    // SW
    direction.col = -1;
    direction.row = -1;
    neighborDirections[5] = direction;
    // W
    direction.col = -1;
    direction.row = 0;
    neighborDirections[6] = direction;
    // NW
    direction.col = -1;
    direction.row = 1;
    neighborDirections[7] = direction;

    // Subscribe precalc function to map change event
    Callback cb = std::bind(&AStarPather::MapChangePrecalcs, this);
    Messenger::listen_for_message(Messages::MAP_CHANGE, cb);


    // Debug Checking sizes
    int size = 0;
    size = sizeof(GridNode*);
    size = sizeof(GridPos);
    size = sizeof(float);
    size = sizeof(OnList);
    size = sizeof(int);
    size = sizeof(OnList*);
    sizeof(short);
    sizeof(OnList&);
    sizeof(GridNode);
    sizeof(TestSizeGridNode);

    //GridNode boi;
    nodeOnLists;


    
    return true; // return false if any errors actually occur, to stop engine initialization
}

void AStarPather::shutdown()
{
    /*
        Free any dynamically allocated memory or any other general house-
        keeping you need to do during shutdown.
    */
    // Frees every node in the WHOLE list
    for (int y = 0; y < MaxMapSize; y++)
    {
        for (int x = 0; x < MaxMapSize; x++)
        {
            delete mNodeMap[y * MaxMapSize + x];
            mNodeMap[y * MaxMapSize + x] = NULL;
        }
    }
}

PathResult AStarPather::compute_path(PathRequest &request)
{
    if (request.newRequest == true)
    {        
        // Clears map and open list
        memset(nodeOnLists, 0, sizeof(OnList) * 40 * 40); // resets the onListArray RILEY NOTE: can decrease to map size
        mOpenList.Clear();
        
        // Save start and end
        mStart = terrain->get_grid_position(request.start);
        mGoal = terrain->get_grid_position(request.goal);

		if (request.settings.debugColoring == true) // Debug color start and end
		{
			terrain->set_color(mStart, Colors::Orange);
			terrain->set_color(mGoal, Colors::Orange);
		}

        // Setup data per request paramaters
        currHeuristic = heuristicFuncs[static_cast<int>(request.settings.heuristic)];

        // Start algorithm

        // Calculates costs for the first node
        GridNode* parentNode = mNodeMap[mStart.row * MaxMapSize + mStart.col];
        parentNode->parent = NULL;
        parentNode->givenCost = 0;
        parentNode->totalCost = currHeuristic(parentNode->pos, mGoal) * request.settings.weight;
        parentNode->whichList = OnList::OpenList;
        if (request.settings.debugColoring == true)
        {
            terrain->set_color(parentNode->pos, DirectX::Colors::Azure);
        }

        // Push node
        mOpenList.PushNode(parentNode);
    }

    while (mOpenList.Empty() == false)
    {
        /* Pop cheapest */
        GridNode* parentNode = mOpenList.PopCheapest();

        /* Check for found path */
        if (parentNode->pos == mGoal) // if we popped off the goal node
        {
            // We've found the path
   
            if (request.settings.rubberBanding == false && request.settings.smoothing == false) // If no pre processing
            {
                // Create the path, from the goal to the beginning
                while (parentNode != 0)
                {
                    request.path.emplace_front(terrain->get_world_position(parentNode->pos));
                    parentNode = parentNode->parent;
                }
            }
            else // Do post processing
            {
                PostProcess(request, parentNode);
            }


            return PathResult::COMPLETE;
        }

        /* Path not found, continue algorithm */
        parentNode->whichList = OnList::ClosedList; // Put parent on the closed list
        if (request.settings.debugColoring == true) // Debug coloring
        {
            terrain->set_color(parentNode->pos, DirectX::Colors::Yellow);
        }

        // Check each viable neighbor
        int neighbor = 1 << 7;
        int i = 0;

        GridNode& newNeighbor = scratchNewNeighbor;
        while (neighbor) // For each neighbor
        {
            if (parentNode->validNeighbors & neighbor) // if current neighbor is valid
            {
                // Gets neighbor position
                newNeighbor.pos.col = parentNode->pos.col + neighborDirections[i].col;
                newNeighbor.pos.row = parentNode->pos.row + neighborDirections[i].row;

               
                // Calculate the given cost
                 // Was using the ternary operator here before to add regular or diagnoal cost
                 // Don't need that thought, just add the additional length * 0 if i is even (cardinal), or * 1 if it's odd (diagonal)
                newNeighbor.givenCost = parentNode->givenCost + 1 + (((float)M_SQRT2 - 1) * (i % 2));

                // Calculates the total cost
                float hCost;
                if (request.settings.heuristic == Heuristic::OCTILE) // If octile, just run it here, don't use the function, for extra speed
                {
                    int yDiff = mGoal.row - newNeighbor.pos.row;
                    if (yDiff < 0)
                    {
                        yDiff *= -1;
                    }

                    int xDiff = mGoal.col - newNeighbor.pos.col;
                    if (xDiff < 0)
                    {
                        xDiff *= -1;
                    }

                    const int minDiff = xDiff > yDiff ? yDiff: xDiff;
                    hCost = (minDiff * sqrt2f) + (const float)(xDiff > yDiff ? xDiff : yDiff)/*max*/ - minDiff;
                }
                else // Use the heuristic function, cause we don't care about speed
                {
                    hCost = currHeuristic(newNeighbor.pos, mGoal);
                }

                newNeighbor.totalCost = newNeighbor.givenCost + (hCost * request.settings.weight); // f(x) = g(x) + (h(x) * weight)

                GridNode* checkNeighbor = mNodeMap[newNeighbor.pos.row * MaxMapSize + newNeighbor.pos.col]; // gets matching neighbor from map
                if (checkNeighbor->whichList == OnList::NoList) // If neighbor is on no list
                {
                    // Put on open list
                    // Sets list variable to open list
                    checkNeighbor->whichList = OnList::OpenList;
                    // Debug coloring
                    if (request.settings.debugColoring == true) 
                    {
                        terrain->set_color(checkNeighbor->pos, DirectX::Colors::Blue);
                    }
                    // Copies info
                    checkNeighbor->parent = parentNode;
                    checkNeighbor->givenCost = newNeighbor.givenCost;
                    checkNeighbor->totalCost = newNeighbor.totalCost;

                    // Pushes node
                    mOpenList.PushNode(checkNeighbor);
                }
                else // On the open or closed list
                {
                    if (newNeighbor.totalCost < checkNeighbor->totalCost) // if new path is cheaper
                    {
                        // Updates info, whether it's on open or closed list
                        checkNeighbor->parent = parentNode;
                        checkNeighbor->givenCost = newNeighbor.givenCost;
                        checkNeighbor->totalCost = newNeighbor.totalCost;

                        if (checkNeighbor->whichList == OnList::ClosedList)
                        {
                            // Put on open list
                            checkNeighbor->whichList = OnList::OpenList; // Sets list variable
                            if (request.settings.debugColoring == true) // Debug coloring
                            {
                                terrain->set_color(checkNeighbor->pos, DirectX::Colors::Blue);
                            }
                            // Pushes node
                            mOpenList.PushNode(checkNeighbor);
                        }

                        // If not on closed list, info was already updated, which is the same thing as replacing it on the open list
                    }
                }
            }
            
            // Increments counters
            ++i;
            neighbor = neighbor >> 1;
        }

        if (request.settings.singleStep == true /* || add check for taking too much time this frame*/)
        {
            return PathResult::PROCESSING;
        }
    }

    // If reaching here, open list is empty, but no path was found, so there is no path
    return PathResult::IMPOSSIBLE; // Return impossible
}

void AStarPather::MapChangePrecalcs()
{

    // Clears map, and calculates valid neighbors for each node
    GridPos checkPos;
    for (int y = 0; y < MaxMapSize; y++)
    {
        for (int x = 0; x < MaxMapSize; x++)
        {
            GridNode* currNode = mNodeMap[y * MaxMapSize + x];

            currNode->whichList = OnList::NoList; // Clears from lists
            currNode->validNeighbors = 0; // Clears neighbors

            int currBit = 1 << 7;
            for (int k = 0; k < 8; k++) // For each possible direction
            {
                // Gets neigbor position from current direction
                checkPos.col = currNode->pos.col + neighborDirections[k].col;
                checkPos.row = currNode->pos.row + neighborDirections[k].row;

                bool neighborValid = terrain->is_valid_grid_position(checkPos) && terrain->is_wall(checkPos) == false;  // If neighbor in grid, and not a wall



                if (((k % 2) > 0) && neighborValid) // if i is odd, we're on a diagonal, and need additional checks
                {
                    bool counterClockwiseGood = true;
                    bool clockwiseGood = true;

                    // Gets index of clockwise direction
                    int counterClockwiseIndex = k - 1;
                    if (counterClockwiseIndex < 0)
                    {
                        counterClockwiseIndex = 7;
                    }

                    // Check direction
                    checkPos.col = currNode->pos.col + neighborDirections[counterClockwiseIndex].col;
                    checkPos.row = currNode->pos.row + neighborDirections[counterClockwiseIndex].row;
                    counterClockwiseGood = terrain->is_valid_grid_position(checkPos) && terrain->is_wall(checkPos) == false;

                    // Get index of clockwise direction
                    int clockwiseIndex = k + 1;
                    if (clockwiseIndex > 7)
                    {
                        clockwiseIndex = 0;
                    }

                    // Check direction
                    checkPos.col = currNode->pos.col + neighborDirections[clockwiseIndex].col;
                    checkPos.row = currNode->pos.row + neighborDirections[clockwiseIndex].row;
                    clockwiseGood = terrain->is_valid_grid_position(checkPos) && terrain->is_wall(checkPos) == false;

                    neighborValid = counterClockwiseGood && clockwiseGood;
                }

                if (neighborValid)
                {
                    currNode->validNeighbors = currNode->validNeighbors | currBit; // Sets the bit of this neighbor as valid
                }


                currBit = currBit >> 1; // Advances bit
            }
        }
    }
}

// void AStarPather::AssignOnLists()
// {
//     // NOTE: Currently not using this approach
//     //int onListPos = 0;

//     //// For each node in the WHOLE list
//     //for (int y = 0; y < MaxMapSize; y++)
//     //{
//     //    for (int x = 0; x < MaxMapSize; x++)
//     //    {
//     //        // Assigns the node a peice of the onNodeList for it's whichList
//     //        mNodeMap[y * MaxMapSize + x]->whichList = &nodeOnLists[onListPos]; 
//     //        onListPos++;
//     //    }
//     //}
// }


// Heuristics
float GetOctileDist(const GridPos& from, const GridPos& to)
{
    int yDiff = to.row - from.row;
    if (yDiff < 0)
    {
        yDiff *= -1;
    }

    int xDiff = to.col - from.col;
    if (xDiff < 0)
    {
        xDiff *= -1;
    }

    int minDiff = std::min(xDiff, yDiff);
    return (minDiff * (float)M_SQRT2) + std::max(xDiff, yDiff) - minDiff;
}

float GetChebyshevDist(const GridPos& from, const GridPos& to)
{
    int yDiff = to.row - from.row;
    if (yDiff < 0)
    {
        yDiff *= -1;
    }

    int xDiff = to.col - from.col;
    if (xDiff < 0)
    {
        xDiff *= -1;
    }

    return float(std::max(xDiff, yDiff));
}

float GetEuclideanDist(const GridPos& from, const GridPos& to)
{
    int yDiff = to.row - from.row;
    if (yDiff < 0)
    {
        yDiff *= -1;
    }

    int xDiff = to.col - from.col;
    if (xDiff < 0)
    {
        xDiff *= -1;
    }

    return std::sqrtf(float(xDiff * xDiff) + float(yDiff * yDiff));
}

float GetManhattanDist(const GridPos& from, const GridPos& to)
{
    int yDiff = to.row - from.row;
    if (yDiff < 0)
    {
        yDiff *= -1;
    }

    int xDiff = to.col - from.col;
    if (xDiff < 0)
    {
        xDiff *= -1;
    }

    return float(xDiff + yDiff);
}

float GetInconsistentDist(const GridPos& from, const GridPos& to)
{
    if ((from.row + from.col) % 2 > 0) // If odd
    {
        return GetEuclideanDist(from, to); // Return regular euclidean distance
    }

    return 0.0f; // If even, just return 0
}



// post processing functions ////////////////////////////////////////////////

void AStarPather::PostProcess(PathRequest& request, GridNode* parentNode)
{
    if (request.settings.rubberBanding == true && request.settings.smoothing == false) // Just rubberbanding
    {
        std::vector<GridPos> processPath;

        // Makes the path
        while (parentNode != 0)
        {
            processPath.insert(processPath.begin(), parentNode->pos);
            parentNode = parentNode->parent;
        }

        RubberBandPath(processPath);

        // Puts rubber banded path into return path
        for (auto it = processPath.begin(); it != processPath.end(); it++)
        {
            request.path.emplace_back(terrain->get_world_position(*it));
        }

    }
    else if (request.settings.rubberBanding == false && request.settings.smoothing == true) // Just smoothing
    {
        std::vector<Vec3> processPath;

        // Makes the path, using world position vec 3s, cause we need floats for smoothing
        while (parentNode != 0)
        {
            processPath.insert(processPath.begin(), terrain->get_world_position(parentNode->pos));
            parentNode = parentNode->parent;
        }

        SmoothPath(processPath);

        // Puts the processed path into the return path
        for (auto it = processPath.begin(); it != processPath.end(); it++)
        {
            request.path.emplace_back(*it);
        }
    }
    else // Both. Both is good
    {

        // Step 1: Do rubber banding
        std::vector<GridPos> rubberBandPath;

        // Makes the path
        while (parentNode != 0)
        {
            rubberBandPath.insert(rubberBandPath.begin(), parentNode->pos);
            parentNode = parentNode->parent;
        }

        RubberBandPath(rubberBandPath);


        std::vector<Vec3> processPath;

        // Step 2: Fill in gaps to make them less than 1.5 width
        for (auto it = rubberBandPath.begin(); it != rubberBandPath.end(); it++)
        {
            processPath.push_back(terrain->get_world_position(*it));
        }

        FillInPath(processPath);

        // Step 3: Smooth filled in path
        SmoothPath(processPath);

        // Puts processes path into return path
        for (auto it = processPath.begin(); it != processPath.end(); it++)
        {
            request.path.emplace_back(*it);
        }
    }
}

void AStarPather::RubberBandPath(std::vector<GridPos>& processPath)
{
    // what I'm doing

    /*
    for each node, starting at the goal
	    using the node 2 spaces back, make a box starting from the goal, and going the x and y difference to the other node.
	    check all nodes in this box, and if none are a wall, delete the middle node

	    continue to go one more back, and repeat the above process until you get a space that is filled

    */

    for (int i = (int)(processPath.size() - 1); i >= 1; i--)
    {
        int j = i - 2; // The node that creates the other edge of our space check box

        if (i == 1)
        {
            j++;
        }

        GridPos furthestNode = processPath[i];
        while (j >= 1)
        {
            // Gets the differences in the grid from the current node to the other node
            int xDiff = processPath[j].col - furthestNode.col;
            int xSign = xDiff < 0 ? -1 : 1;
            int yDiff = processPath[j].row - furthestNode.row;
            int ySign = yDiff < 0 ? -1 : 1;

            bool spaceClear = true;
            GridPos checkNode = furthestNode;
            // Goes through the bounding box between the two noced
            for (int y = 0; y <= std::abs(yDiff); y++) // For each row between furthest node and other node
            {
                for (int x = 0; x <= std::abs(xDiff); x++) // For each column
                {
                    // Translate to the current node we're checking
                    checkNode.row = furthestNode.row + (y * ySign);
                    checkNode.col = furthestNode.col + (x * xSign);

                    if (terrain->is_wall(checkNode)) // If we've found a wall
                    {
                        // Can't rubber band, go directly to the end
                        spaceClear = false;
                        goto space_clear_check;
                    }
                }
            }

        space_clear_check:
            if (spaceClear == false)
            {
                break; // Space is not clear, break out and move on to the next i
            }
            else // Space is clear
            {
                processPath.erase(processPath.begin() + (j + 1)); // rubber band away the middle node

                // Update indicies due to erase
                i--;
                j--;
            }

        }
    }

    // Done rubber banding, and the path was passed by reference, so updated path is already returned
}

void AStarPather::SmoothPath(std::vector<Vec3>& processPath)
{
    int p1Index = 0;
    int p2Index = 0;
    int p3Index = 1;
    int p4Index = 2;
    p4Index = std::clamp(p4Index, 0, (int)processPath.size() - 1); // Ensures we don't read outsize the array

    Vec3 currSplinePoint; // Scratch point to have Vec3::CatmulRom return into
    float t = 0.25f; // Start at .25, and we will move t by that much after each spline point

    while (p3Index < processPath.size()) // p3 index gets incremented past size of list when algorith finishes, so go until then
    {
        /*Do one spline */
        while (t < 1)
        {
            Vec3::CatmullRom(processPath[p1Index], processPath[p2Index],
                processPath[p3Index], processPath[p4Index], t, currSplinePoint);

            processPath.insert(processPath.begin() + p3Index, currSplinePoint); // Inserts new spline point

            // Only move p3 and p4, because points are being placed before them
            p4Index++;
            p3Index++;

            t += 0.25f; // Update t
        }
        t = 0.25f; // Resets t


        /* Advance indices */
        if (p1Index == p2Index) // if moving on from the very beginning
        {
            p2Index += 4; // Only move p2 forward (need 2 move 4 because of the points that were placed
        }
        else // If in the middle or end
        {
            // Move both p1 and p2 forward
            p1Index += 4;
            p2Index += 4;
        }

        p3Index++; // Only move by 1 on these because no spline points have been placed after them
        p4Index++;

        if (p4Index >= processPath.size()) // If last index goes beyond the end
        {
            p4Index--; // Bring it back
        }
    }

    // Done with smoothing, and path was passed by reference, so smoothed path is already returned
}

void AStarPather::FillInPath(std::vector<Vec3>& processPath)
{
    int leftIndex = 0;
    int rightIndex = 1;
    Vec3 fillPoint; // Scratch point to put the filled point into

    GridPos pos1;
    pos1.row = 0;
    pos1.col = 0;
    GridPos pos2;
    pos2.row = 0;
    pos2.col = 1;
    float tileSize = std::abs(Vec3::Distance(terrain->get_world_position(pos1), terrain->get_world_position(pos2))); // gets the tile size
    float gapThreshold = tileSize * 1.5f; // Gets 1.5 tile size, as that's the threshold the slides say to use for when to fill in points

    while (rightIndex < processPath.size())
    {
        float distance = std::abs(Vec3::Distance(processPath[rightIndex], processPath[leftIndex]));
        float maybeTileWidth = terrain->get_terrain_plane().w;
        if (distance > gapThreshold) // If distance is greater than 3 (1.5 tiles)
        {
            // Calculate midpoint to fill in between right and left
            Vec3 directionToRight = processPath[rightIndex] - processPath[leftIndex];
            fillPoint = processPath[leftIndex] + (directionToRight / 2);

            processPath.insert(processPath.begin() + rightIndex, fillPoint); // Inserts midpoint

            // Don't update right index, as after inserting, it is now the index of the new fill node
        }
        else
        {
            // Moves on to next set of points
            leftIndex++;
            rightIndex++;
        }
    }

    // Filling in is done, and since we passed the path by reference, filled in path is already returned
}

// Debug functions ///////////////////////////////////////////////////////////////////////////////////////

bool AStarPather::MadeInfiniteParentLoop(GridNode* nodeToCheck)
{
    int parentPathSize = 0;

    int parentPathMax = 40 * 40;
    GridNode* currNode = nodeToCheck;
    while (currNode != NULL && parentPathSize < parentPathMax) // While there are parents left, and not at max size
    {
        // Advance along parent path
        currNode = currNode->parent;
        parentPathSize++;
    }

    if (parentPathSize >= parentPathMax) // If we're in an infinite loop
    {
        return true;
    }

    return false;
}

bool AStarPather::WillMakeInfiniteLoop(GridNode* nodeToAddTo, GridNode* nodeToAdd)
{
    GridNode* oldParent = nodeToAddTo->parent;

    nodeToAddTo->parent = nodeToAdd;

    if (MadeInfiniteParentLoop(nodeToAddTo))
    {
        std::cout << "It is here";
        nodeToAddTo->parent = oldParent;
        return true;
    }

    nodeToAddTo->parent = oldParent;
    return false;
}

GridNode::GridNode(OnList* newOnList)
    : whichList(*newOnList)
{
    parent = NULL;
    pos = GridPos();
    totalCost = 0;
    givenCost = 0;
    validNeighbors = 0;
}
