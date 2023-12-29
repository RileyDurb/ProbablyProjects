/***********************************************************************************************************************
filename    P3_TerrainAnalysis.cpp
author      Riley Durbin
email		ridurbin0@gmail.com

Brief Description:
  Contains functions for analysing a 2D grid in different ways. Meant for use with an external engine, which provides
  the map that gets analyzed and colored to show the analysis. The engine also provides functionality for visualizing
  the agents, whose perspectives are used for LOS in some of these functions.

***********************************************************************************************************************/

#include <pch.h>
#include "Terrain/TerrainAnalysis.h"
#include "Terrain/MapMath.h"
#include "Agent/AStarAgent.h"
#include "Terrain/MapLayer.h"
#include "Projects/ProjectThree.h"

#include <iostream>

// Constants /////////////////////////////////////////////////////////////////////////
const float losLineFluff = .05f; // Litte boost given to the los lines on walls, so that passing through the corner of a wall won't make you see past it

const std::vector<GridPos> adjacentCardinalDirections = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} }; // N, E, S, W
const std::vector<GridPos> adjacentAllDirections = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} }; // N, NE E, SE, S, SW, W, NW

const float losDotProductMin = -0.1f;
//////////////////////////////////////////////////////////////////////////////////////

bool ProjectThree::implemented_fog_of_war() const // extra credit
{
    return false;
}

// Helper functions //////////////////////////////////////////////////////////////////
float GetEuclideanDist(int fromCol, int fromRow, int toCol, int toRow)
{
    int yDiff = toRow - fromRow;
    if (yDiff < 0)
    {
        yDiff *= -1;
    }

    int xDiff = toCol - fromCol;
    if (xDiff < 0)
    {
        xDiff *= -1;
    }

    return std::sqrtf(float(xDiff * xDiff) + float(yDiff * yDiff));
}

// returns a tuple of two points, left is the left point in the line, right is the right point
std::tuple<Vec2, Vec2> GetDiagonalLineTopLeftBottomRight(const Vec3& startPoint, float tileSize)
{

    // Make los wall lines for p1
    Vec2 diagonalLeft = Vec2(startPoint.x - (tileSize / 2) - losLineFluff, startPoint.z + (tileSize / 2) + losLineFluff);
    Vec2 diagonalRight = Vec2(startPoint.x + (tileSize / 2) + losLineFluff, startPoint.z - (tileSize / 2) - losLineFluff);

    return std::make_tuple(diagonalLeft, diagonalRight);
}

// returns a tuple of two points, left is the left point in the line, right is the right point
std::tuple<Vec2, Vec2> GetDiagonalLineBottomLeftTopRight(const Vec3& startPoint, float tileSize)
{

    // Make los wall lines for p1
    Vec2 diagonalLeft = Vec2(startPoint.x - (tileSize / 2) - losLineFluff, startPoint.z - (tileSize / 2) - losLineFluff);
    Vec2 diagonalRight = Vec2(startPoint.x + (tileSize / 2) + losLineFluff, startPoint.z + (tileSize / 2) + losLineFluff);

    return std::make_tuple(diagonalLeft, diagonalRight);
}

void MarkWithOpenness(const GridPos& tile, MapLayer<float>& layer)
{
    // Marks tile
    float closestWallDistance = distance_to_closest_wall(tile.row, tile.col);
    layer.set_value(tile, 1 / (closestWallDistance * closestWallDistance));
}

// k is index of direction, from 0 at N, counterclockwise to 7 at NE
bool IsValidNeighbor(const GridPos& checkTile, int k)
{
    GridPos currPos;

    // Gets neigbor position from current direction
    currPos.col = checkTile.col + adjacentAllDirections[k].col;
    currPos.row = checkTile.row + adjacentAllDirections[k].row;

    bool neighborValid = terrain->is_valid_grid_position(currPos) && terrain->is_wall(currPos) == false;  // If neighbor in grid, and not a wall

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
        currPos.col = checkTile.col + adjacentAllDirections[counterClockwiseIndex].col;
        currPos.row = checkTile.row + adjacentAllDirections[counterClockwiseIndex].row;
        counterClockwiseGood = terrain->is_valid_grid_position(currPos) && terrain->is_wall(currPos) == false;

        // Get index of clockwise direction
        int clockwiseIndex = k + 1;
        if (clockwiseIndex > 7)
        {
            clockwiseIndex = 0;
        }

        // Check direction
        currPos.col = checkTile.col + adjacentAllDirections[clockwiseIndex].col;
        currPos.row = checkTile.row + adjacentAllDirections[clockwiseIndex].row;
        clockwiseGood = terrain->is_valid_grid_position(currPos) && terrain->is_wall(currPos) == false;

        neighborValid = counterClockwiseGood && clockwiseGood;
    }

    return neighborValid;
}

float GetMaxNeighborInfluence(MapLayer<float>& layer, GridPos tile, float decay, bool dualOccupancy = false)
{
    int neighborRow = 0;
    int neighborCol = 0;
    float maxNeighborInfluence = 0;
    for (int k = 0; k < 8; k++)
    {
        neighborRow = tile.row + adjacentAllDirections[k].row;
        neighborCol = tile.col + adjacentAllDirections[k].col;

        if (IsValidNeighbor(tile, k) == false) continue; // Neighbor is invalid, skip it

        float neighborInfluence = layer.get_value(neighborRow, neighborCol); // Gets current influence
        float distance = (k % 2) == 0 ? 1 : (float)M_SQRT2; // Distance, 1 if direction is cardinal ( k is even), sqrt 2 if direction is diagonal (k is odd)
        neighborInfluence = (neighborInfluence * exp(-1 * distance * decay)); // Applies decay

        if (dualOccupancy == true)
        {
            // uses abs, as values can be from -1 to 1
            if (std::abs(neighborInfluence) > std::abs(maxNeighborInfluence)) // If we've got a new max for all neighbors
            {
                maxNeighborInfluence = neighborInfluence; // Saves new max
            }
        }
        else
        {
            if (neighborInfluence > maxNeighborInfluence) // If we've got a new max for all neighbors
            {
                maxNeighborInfluence = neighborInfluence; // Saves new max
            }
        }

    }

    return maxNeighborInfluence;
}
///////////////////////////////////////////////////////////////////////////////////////
// Analysis Functions /////////////////////////////////////////////////////////////////

float distance_to_closest_wall(int row, int col)
{
    /*
        Check the euclidean distance from the given cell to every other wall cell,
        with cells outside the map bounds treated as walls, and return the smallest
        distance.  Make use of the is_valid_grid_position and is_wall member
        functions in the global terrain to determine if a cell is within map bounds
        and a wall, respectively.
    */

    float minDist = std::numeric_limits<float>::max();

    for (int i = -1; i < terrain->get_map_width() + 1; i++)
    {
        for (int j = -1; j < terrain->get_map_height() + 1; j++)
        {
            if (i == col && j == row) continue; // Skips if it's the tile we're checking from

            // If outside the map, or a wall
            if (terrain->is_valid_grid_position(j, i) == false || terrain->is_wall(j, i) == true)
            {
                float currDist = GetEuclideanDist(col, row, i, j);
                if (currDist < minDist) // if we've found a new min distance
                {
                    minDist = currDist; // Save the new min
                }

            }
        }
    }
    return minDist;
}

bool is_clear_path(int row0, int col0, int row1, int col1)
{
    /*
        Two cells (row0, col0) and (row1, col1) are visible to each other if a line
        between their centerpoints doesn't intersect the four boundary lines of every
        wall cell.  You should puff out the four boundary lines by a very tiny amount
        so that a diagonal line passing by the corner will intersect it.  Make use of the
        line_intersect helper function for the intersection test and the is_wall member
        function in the global terrain to determine if a cell is a wall or not.
    */


    Vec3 worldPosP1 = terrain->get_world_position(row0, col0);
    Vec3 worldPosP2 = terrain->get_world_position(row1, col1);

    // Los line is these two above points, just as Vec2s
    Vec2 losLineP1 = Vec2(worldPosP1.x, worldPosP1.z);
    Vec2 losLineP2 = Vec2(worldPosP2.x, worldPosP2.z);

    if (terrain->is_wall(row0, col0) || terrain->is_wall(row1, col1)) // Needs to check for if starting points are walls, because los line doesn't always cross through the x lines on the starting points
    {
        return false;
    }

    

    // Calculate tile size
    GridPos pos1;
    pos1.row = 0;
    pos1.col = 0;
    GridPos pos2;
    pos2.row = 0;
    pos2.col = 1;
    float tileSize = std::abs(Vec3::Distance(terrain->get_world_position(pos1), terrain->get_world_position(pos2))); // gets the tile size (distance between two adjacent tiles)
    
    // Calculates info for the box to check
    int losBoxWidth = std::abs(col1 - col0) + 1;
    int losBoxHeight = std::abs(row1 - row0) + 1;

    GridPos checkPos; // Scratch vector to be translated in our loop, to each potential wall position
    int xDir = col0 < col1 ? -1 : 1; // X direction to iterate in
    int yDir = row0 < row1 ? -1 : 1; // y direction to iterate in

    int blocked = false;
    // Check box containing the los line
    for (int i = 0; i < losBoxWidth; i++)
    {
        for (int j = 0; j < losBoxHeight; j++)
        {

            // Translates from p2 to the current tile to check
            checkPos.col = col1 + (i * xDir);
            checkPos.row = row1 + (j * yDir);

            //if ((checkPos.col == col0 && checkPos.row == row0) || checkPos.col == col1 && checkPos.row == row1) continue; // Skip points we're checking los for. CORRECTION: do, cause viewpoint could be a wall

            if (terrain->is_wall(checkPos.row, checkPos.col))
            {
                // Check los
                Vec3 worldPosCheckTile = terrain->get_world_position(checkPos.row, checkPos.col);

                // Checks first diagonal
                std::tuple<Vec2, Vec2> diagonalLine = GetDiagonalLineTopLeftBottomRight(worldPosCheckTile, tileSize);

                blocked = line_intersect(losLineP1, losLineP2, diagonalLine._Myfirst._Val, diagonalLine._Get_rest()._Myfirst._Val);

                if (blocked)
                {
                    return false; // LOS not clear, returns false
                }

                // Checks second diagonal
                diagonalLine = GetDiagonalLineBottomLeftTopRight(worldPosCheckTile, tileSize);
                
                blocked = line_intersect(losLineP1, losLineP2, diagonalLine._Myfirst._Val, diagonalLine._Get_rest()._Myfirst._Val);
                if (blocked)
                {
                    return false;
                }
            }
        }
    }

    return true; // if reaching here, path not blocked
}


void analyze_openness(MapLayer<float>& layer)
{
    /*
        Mark every cell in the given layer with the value 1 / (d * d),
        where d is the distance to the closest wall or edge.  Make use of the
        distance_to_closest_wall helper function.  Walls should not be marked.
    */

    GridPos scratchGridPos;
    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_width(); j++)
        {
            if (terrain->is_wall(j, i) == false) // If tile is a wall
            {
                scratchGridPos.row = j;
                scratchGridPos.col = i;

                MarkWithOpenness(scratchGridPos, layer);
            }
        }
    }
}



void analyze_visibility(MapLayer<float> &layer)
{
    /*
        Mark every cell in the given layer with the number of cells that
        are visible to it, divided by 160 (a magic number that looks good).  Make sure
        to cap the value at 1.0 as well.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
        helper function.
    */


    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            int numVisibleTiles = 0;
            for (int k = 0; k < terrain->get_map_width(); k++)
            {
                for (int l = 0; l < terrain->get_map_height(); l++)
                {
                    if (i == k && j == l) continue; // Don't check for los with the same tile

                    if (is_clear_path(j, i, l, k) == true)
                    {
                        numVisibleTiles++;
                    }
                }
            }

            // Calculates and sets visibility score for the current tile j i
            float visibilityScore = static_cast<float>(numVisibleTiles) / 160.0f; // Calculates visibility score using suggested magic number
            visibilityScore = std::clamp(visibilityScore, 0.0f, 1.0f); // Clamps score
            layer.set_value(j, i, visibilityScore); // Sets visibility score
        }
    }
}



void analyze_visible_to_cell(MapLayer<float> &layer, int row, int col)
{
    /*
        For every cell in the given layer mark it with 1.0
        if it is visible to the given cell, 0.5 if it isn't visible but is next to a visible cell,
        or 0.0 otherwise.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
        helper function.
    */

    // 1st pass to mark if visible or not
    GridPos scratchGridPos;
    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            bool visibleToTarget = is_clear_path(row, col, j, i);

            scratchGridPos.row = j;
            scratchGridPos.col = i;
            if (visibleToTarget)
            {
                layer.set_value(scratchGridPos, 1.0f);
            }
            else
            {
                layer.set_value(scratchGridPos, 0.0f);
            }
        }
    }

    // Second pass to mark if next to a visible tile
    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            if (layer.get_value(j, i) == 0.0f) // If it might be marked as partially visible
            {
                if (terrain->is_wall(j, i) == true) continue; // Skip if is a wall, can't be partially visible

                int neighborCol = 0;
                int neighborRow = 0;
                // Check all neighbors of current tile
                for (int k = 0; k < 8; k++)
                {
                    neighborCol = i + adjacentAllDirections[k].col;
                    neighborRow = j + adjacentAllDirections[k].row;

                    if (IsValidNeighbor({ j, i }, k) && layer.get_value(neighborRow, neighborCol) == 1.0f) // If this neighbor is visible
                    {
                        // Marks the current tile as partially visible
                        scratchGridPos.row = j;
                        scratchGridPos.col = i;
                        layer.set_value(scratchGridPos, 0.5f);

                        break; // Only need one neighbor to be visible, break out
                    }
                }
            }
        }
    }
}

void analyze_agent_vision(MapLayer<float> &layer, const Agent *agent)
{
    /*
        For every cell in the given layer that is visible to the given agent,
        mark it as 1.0, otherwise don't change the cell's current value.

        You must consider the direction the agent is facing.  All of the agent data is
        in three dimensions, but to simplify you should operate in two dimensions, the XZ plane.

        Take the dot product between the view vector and the vector from the agent to the cell,
        both normalized, and compare the cosines directly instead of taking the arccosine to
        avoid introducing floating-point inaccuracy (larger cosine means smaller angle).

        Give the agent a field of view slighter larger than 180 degrees.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
        helper function.
    */

    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            // Calculates vector from agent to tile
            Vec3 targetTileFullPos = terrain->get_world_position(j, i);
            Vec2 targetTilePos = Vec2(targetTileFullPos.x, targetTileFullPos.z);

            Vec3 agentFullPos = agent->get_position();
            Vec2 agentPos = Vec2(agentFullPos.x, agentFullPos.z);

            Vec2 agentToTileVector = targetTilePos - agentPos;

            // Gets agent view vector
            Vec3 agentViewFullVector = agent->get_forward_vector();

            Vec2 agentViewVector = Vec2(agentViewFullVector.x, agentViewFullVector.z);

            // Dot product between view and agent to tile vectors
            float dotProduct = agentViewVector.Dot(agentToTileVector);

            GridPos agentGridPos = terrain->get_grid_position(agent->get_position());
            if (dotProduct > losDotProductMin && is_clear_path(agentGridPos.row, agentGridPos.col, j, i)) // if dot product a little greater than 180 degrees, and visible to agent
            {
                // In line of sight
                layer.set_value({ j, i }, 1.0f); // Sets value
            }

        }
    }
}


void propagate_solo_occupancy(MapLayer<float> &layer, float decay, float growth)
{
    /*
        For every cell in the given layer:

            1) Get the value of each neighbor and apply decay factor
            2) Keep the highest value from step 1
            3) Linearly interpolate from the cell's current value to the value from step 2
               with the growing factor as a coefficient.  Make use of the lerp helper function.
            4) Store the value from step 3 in a temporary layer.
               A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

        After every cell has been processed into the temporary layer, write the temporary layer into
        the given layer;
    */
    
    MapLayer<float> tempLayer = layer;
    
    tempLayer.for_each([](float& value) { value = 0.0f; }); // Zeros out each value

    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            if (terrain->is_wall(j, i)) continue; // Don't propagate through walls

            float maxNeighborInfluence = GetMaxNeighborInfluence(layer, { j, i }, decay);

            float newInfluence = lerp(layer.get_value(j, i), maxNeighborInfluence, growth); // lerp from current value to max neighbor influence

            tempLayer.set_value(j, i, newInfluence);
        } 
    }

    // Sets temp layer to actual layer
    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            layer.set_value(j, i, tempLayer.get_value(j, i));
        }
    }
}

void propagate_dual_occupancy(MapLayer<float> &layer, float decay, float growth)
{
    /*
        Similar to the solo version, but the values range from -1.0 to 1.0, instead of 0.0 to 1.0

        For every cell in the given layer:

        1) Get the value of each neighbor and apply decay factor
        2) Keep the highest ABSOLUTE value from step 1
        3) Linearly interpolate from the cell's current value to the value from step 2
           with the growing factor as a coefficient.  Make use of the lerp helper function.
        4) Store the value from step 3 in a temporary layer.
           A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

        After every cell has been processed into the temporary layer, write the temporary layer into
        the given layer;
    */

    MapLayer<float> tempLayer = layer;

    //tempLayer.for_each([](float& value) { value = 0.0f; }); // Zeros out each value

    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            if (terrain->is_wall(j, i)) continue; // Don't propagate through walls

            float maxNeighborInfluence = GetMaxNeighborInfluence(layer, { j, i }, decay, true);

            float newInfluence = lerp(layer.get_value(j, i), maxNeighborInfluence, growth); // lerp from current value to max neighbor influence

            tempLayer.set_value(j, i, newInfluence);
        }
    }

    // Sets temp layer to actual layer
    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            layer.set_value(j, i, tempLayer.get_value(j, i));
        }
    }
}

void normalize_solo_occupancy(MapLayer<float> &layer)
{
    /*
        Determine the maximum value in the given layer, and then divide the value
        for every cell in the layer by that amount.  This will keep the values in the
        range of [0, 1].  Negative values should be left unmodified.
    */

    float maxOccupancyValue = 0;
    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            if (layer.get_value(j, i) > maxOccupancyValue)
            {
                maxOccupancyValue = layer.get_value(j, i);
            }
        }
    }

    if (maxOccupancyValue != 0) // Don't zero out everything if max value is 0, cause that stops things from propagating the first time, probably cause of weird things when you divide by 0
    {
        layer.for_each([maxOccupancyValue](float& value) {
            if (value >= 0)
                value /= maxOccupancyValue;
            }
        ); // Normalize each non-negative value based on max occupancy
    }
}

void normalize_dual_occupancy(MapLayer<float> &layer)
{
    /*
        Similar to the solo version, but you need to track greatest positive value AND 
        the least (furthest from 0) negative value.

        For every cell in the given layer, if the value is currently positive divide it by the
        greatest positive value, or if the value is negative divide it by -1.0 * the least negative value
        (so that it remains a negative number).  This will keep the values in the range of [-1, 1].
    */

    float maxOccupancyValue = 0;
    float minOccupancyValue = 0;
    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            if (layer.get_value(j, i) > maxOccupancyValue) // if new max
            {
                maxOccupancyValue = layer.get_value(j, i); // Saves new max
            }

            if (layer.get_value(j, i) < minOccupancyValue) // If new min
            {
                minOccupancyValue = layer.get_value(j, i); // Saves new min
            }
        }
    }

    // Normalize each value
    layer.for_each([maxOccupancyValue, minOccupancyValue](float& value) {
        if (value >= 0) value /= maxOccupancyValue; // Use max for nonnegatives
        else value /= (- 1.0f * minOccupancyValue); // use min (* -1 to keep it negative) for negatives
        }
    );
}

void enemy_field_of_view(MapLayer<float> &layer, float fovAngle, float closeDistance, float occupancyValue, AStarAgent *enemy)
{
    /*
        First, clear out the old values in the map layer by setting any negative value to 0.
        Then, for every cell in the layer that is within the field of view cone, from the
        enemy agent, mark it with the occupancy value.  Take the dot product between the view
        vector and the vector from the agent to the cell, both normalized, and compare the
        cosines directly instead of taking the arccosine to avoid introducing floating-point
        inaccuracy (larger cosine means smaller angle).

        If the tile is close enough to the enemy (less than closeDistance),
        you only check if it's visible to enemy.  Make use of the is_clear_path
        helper function.  Otherwise, you must consider the direction the enemy is facing too.
        This creates a radius around the enemy that the player can be detected within, as well
        as a fov cone.
    */

    layer.for_each([](float& value) { if (value < 0) value = 0; }); // Reset all negative values


    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            
            // Calculates vector from agent to tile
            Vec3 targetTileFullPos = terrain->get_world_position(j, i); // Gets position in world of target tile

            Vec2 targetTilePos = Vec2(targetTileFullPos.x, targetTileFullPos.z); // converts to vec 2

            Vec3 agentFullPos = enemy->get_position(); // gets enemey's position
            Vec2 agentPos = Vec2(agentFullPos.x, agentFullPos.z); // Converts to vec 2

            Vec2 agentToTileVector = targetTilePos - agentPos; // Cets direction vector from agent to current target tile

            // Does check for vision circle
            
            // Gets both grid positions as vec2s
            GridPos agentGridPos = terrain->get_grid_position(enemy->get_position());
            Vec2 agentGridPosVec = Vec2((float)agentGridPos.col, (float)agentGridPos.row);
            Vec2 targetTileGridPos = Vec2((float)i, (float)j);

            // Gets distance vector in grid distance
            Vec2 gridDistance = targetTileGridPos - agentGridPosVec;
            if (gridDistance.Length() < closeDistance) // If within vision circle
            {
                GridPos agentGridPos = terrain->get_grid_position(enemy->get_position());

                if (is_clear_path(agentGridPos.row, agentGridPos.col, j, i)) // Ignore vision cone, just check clear path
                {
                    layer.set_value(j, i, occupancyValue);
                }

                continue; // No need to check los since we're within vision circle, continue to next tile    
            }

            // Gets agent view vector
            Vec3 agentViewFullVector = enemy->get_forward_vector();
            Vec2 agentViewVector = Vec2(agentViewFullVector.x, agentViewFullVector.z);

            // Dot product between view and agent to tile vectors
            float dotProduct = agentViewVector.Dot(agentToTileVector);
            float visionCos = (dotProduct / (agentViewVector.Length() * agentToTileVector.Length()));

            float visionTarget = cosf(((2 * (float)M_PI) / 180.0f) * (fovAngle / 4)); // Gets the target cos based on given fov angle


            if (visionCos > visionTarget && is_clear_path(agentGridPos.row, agentGridPos.col, j, i)) // if dot product greater than 180 degrees, and visible to agent
            {
                // In line of sight
                layer.set_value(j, i, occupancyValue); // Sets value
            }

        }
    }
}

bool enemy_find_player(MapLayer<float> &layer, AStarAgent *enemy, Agent *player)
{
    /*
        Check if the player's current tile has a negative value, ie in the fov cone
        or within a detection radius.
    */

    const auto &playerWorldPos = player->get_position();

    const auto playerGridPos = terrain->get_grid_position(playerWorldPos);

    // verify a valid position was returned
    if (terrain->is_valid_grid_position(playerGridPos) == true)
    {
        if (layer.get_value(playerGridPos) < 0.0f)
        {
            return true;
        }
    }

    // player isn't in the detection radius or fov cone, OR somehow off the map
    return false;
}

bool enemy_seek_player(MapLayer<float> &layer, AStarAgent *enemy)
{
    /*
    Peudocode:
        highest value counter
        vector of nodes with the highest value

        for each node
            if current value is higher than saved highest
                clear highest node vector
                save new highest value
                push position at highest value into vector
            else if current value equal to highest
                push node into highest node vector

        if highest node vector has more than one nodes, find the one with the closest distance
        if there are ones with the same closest distance, just pick the one at the beginning of the vector

        also handle all 0
    */
    
    float highestInfluenceValue = 0;
    std::vector<GridPos> highestValueTiles;

    for (int i = 0; i < terrain->get_map_width(); i++)
    {
        for (int j = 0; j < terrain->get_map_height(); j++)
        {
            float currValue = layer.get_value(j, i);
            
            if (currValue == 0) continue; // Don't count nonzero values

            if (currValue > highestInfluenceValue) // if new max
            {
                highestValueTiles.clear(); // Clears all old highest value tiles

                highestInfluenceValue = currValue; // Saves new max value
                highestValueTiles.push_back({ j, i });
            }
            else if (currValue == highestInfluenceValue) // If the same as the current max
            {
                highestValueTiles.push_back({ j, i }); // Just push into vector of highest value tiles
            }
        }
    }

    if (highestValueTiles.empty()) // If found no highest tiles
    {
        return false; // Tile could not be found, return false
    }
    else if (highestValueTiles.size() == 1) // if just one highest value tile
    {
        enemy->path_to(terrain->get_world_position(highestValueTiles[0])); // Sets enemy path to found tile

        return true; // Highest value cell could be found, returns true
    }
    else // Highest value found, but more than one shared the value
    {
        float shortestDistance = std::numeric_limits<float>::max();
        GridPos closestHighValueTile = highestValueTiles[0];

        GridPos enemyPos = terrain->get_grid_position(enemy->get_position());
        float currDistance = 0;
        for (int i = 0; i < highestValueTiles.size(); i++)
        {
            currDistance = std::abs(GetEuclideanDist(enemyPos, highestValueTiles[i]));

            if (currDistance < shortestDistance)
            {
                shortestDistance = currDistance;
                closestHighValueTile = highestValueTiles[i];
            }
        }

        // Now we have the tile closest to the enemy
        enemy->path_to(terrain->get_world_position(closestHighValueTile)); // make enemy path to closest high value tile

        return true; // High value tile could be found, return true;
    }

        

    return false; // If reaching here, a case was not caught, as we should be handling both finding and missing a target
}
