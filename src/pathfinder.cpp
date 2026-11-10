#include <raylib.h>
#include <unordered_map>
#include <cmath>


#include "pathfinder.h"

// Helper function to convert from the CDT navmesh index of a triangle, to the set of three points making up said triangle.
std::vector<Vector2> PathFinder::GetTrianglePoints(const CDT::Triangle& triangle, const std::vector<CDT::V2d<double>> meshPoints) 
{
    // First get the indexes of all the points from their set in CDT.
    size_t i0 = triangle.vertices[0];
    size_t i1 = triangle.vertices[1];
    size_t i2 = triangle.vertices[2];

    // Grab the actual CDT vectors for each point.
    const CDT::V2d<double> c0 = meshPoints[i0];
    const CDT::V2d<double> c1 = meshPoints[i1];
    const CDT::V2d<double> c2 = meshPoints[i2];

    // Convert V2d point to Vector2 point to allow for raylib collision detection.
    Vector2 v0 = { static_cast<float>(c0.x), static_cast<float>(c0.y) };
    Vector2 v1 = { static_cast<float>(c1.x), static_cast<float>(c1.y) };
    Vector2 v2 = { static_cast<float>(c2.x), static_cast<float>(c2.y) };

    return {v0, v1, v2};
};

// Small function to find the middle point of a triangle.
// This is the current method of path following for a path finding agent.
// It is TEMPORARY, for TESTING purposes.
Vector2 PathFinder::GetTriangleMiddlePoint(const std::vector<Vector2>& triangle) 
{
    Vector2 Middle;
    // Simple average out all 3 positions to get the middle point.
    Middle.x = (triangle[0].x + triangle[1].x + triangle[2].x) / 3;
    Middle.y = (triangle[0].y + triangle[1].y + triangle[2].y) / 3;
    return Middle;
}

// This complex function is responsible for generating a path from a target position on the CDT generated navmesh.
// It is specifically a version of the BFS, Breadth First Search algorithm.
// It generates both a set of points (indexes on the navmesh) for an agent to follow, aswell as the set of matching lines to draw.
// It is a very bulky function, to be later broken up into pieces for easier modification.
// As such it is a TEMPORARY function for TESTING purposes.
void PathFinder::GeneratePath(Vector2 targetPosition,const std::vector<CDT::Triangle>& meshIndexes,const std::vector<CDT::V2d<double>>& meshPoints) 
{
    // First we clear the previous path if there indeed was one.
    path.clear();
    linePath.clear();

    // A few logic check variables.
    bool foundCurrent = false;
    bool foundTarget = false;
    bool pathable = false;

    // Initial triangles indexes initialised.
    int targetTriangle = -1;
    int currentTriangle = -1;

    // Vector of the 3 points of a triangle.
    std::vector<Vector2> triangle;
    // Vector of indexes in the current pool, that is the next triangles to check along the path.
    std::vector<int> pool;

    // This lets us map each triangle directly to its distance from the origin point.
    // Distance is currently calculated in the amount of triangles away, not in cartesian coordinates.
    std::unordered_map<int, int> searched;

    // Small helper lambda function to verify the existance of neighbours of a triangle.
    auto GetValidNeighbors = [&](int triIdx) {
        std::vector<int> validNeighbors;
        for (const unsigned int& neighbour : meshIndexes[triIdx].neighbors)
        {
            if (neighbour != CDT::noNeighbor) { 
                validNeighbors.push_back(neighbour); 
            }
        }
        return validNeighbors;
    };
    
    // Verification step.
    // We first see whether or not the current and target positions are in the navmesh at all.
    // If so, we assign them triangle indexes and set pathable to true, allowing for path generation.
    for (size_t i = 0; i < meshIndexes.size(); i++) 
    {
        // We get the actual vector2 points of the triangle on the mesh.
        triangle = GetTrianglePoints(meshIndexes[i], meshPoints);

        // It is so we can check if the designated point is within the triangle by directly
        // using Raylib's built in collision detection.

        // Check for start point.
        if (CheckCollisionPointTriangle(position, triangle[0], triangle[1], triangle[2]))
        {
            currentTriangle = static_cast<int>(i);
            foundCurrent = true;
        }

        // Check for end point.
        if (CheckCollisionPointTriangle(targetPosition, triangle[0], triangle[1], triangle[2])) 
        {
            targetTriangle = static_cast<int>(i);
            foundTarget = true;
        }

        pathable = (foundCurrent && foundTarget);
        // Stop iterating through all triangles once both start and end found as a minor optimisation.
        if (pathable) {break;}
    }
    
    // If there is indeed a path...
    if (pathable) 
    {
        // We set the initial pool to check in the first wave as the neighbours of the starting triangle.
        pool = GetValidNeighbors(currentTriangle);
        // And set the distance of the starting triangle to 0 for later path following.
        searched[currentTriangle] = 0;

        // Initialise the neighbours at distance of 1, this is to allow for pathfinding between,
        // neighbouring triangles.
        for (const int& neighbour : pool) { searched[neighbour] = 1;}

        // So long as we have not yet exhausted all triangles we could possibly path to,
        // or finally reached the target.....
        while (!pool.empty()) 
        {
            // newPool stores the pools of candidates for the next wave.
            std::vector<int> newPool;
            
            // Then we run through every candidate in the current wave pool.
            for (const int& current : pool) 
            {   
                // If we found the target...
                if (current == targetTriangle) 
                {
                    // We clear the next, now useless pool and break the loop forcefully.
                    newPool.clear(); 
                    break; 
                }

                // If the current candidate is not yet the target,
                // then we try to push its neighbours into the next wave pool.
                std::vector<int> toTry = GetValidNeighbors(current);

                for (const int& attempt : toTry) 
                {
                    // Only add them if they are yet to be added.
                    if (searched.find(attempt) == searched.end()) 
                    {
                        newPool.push_back(attempt);
                        // Ensure all new added candidates get an increased distance aswell.
                        searched[attempt] = searched[current] + 1;
                    }
                }
            }

            // Reset pool every wave.
            pool = newPool;
        }

        // Now that the target has been found, we construct the path by going from the target and decrementing distance.
        int current = targetTriangle;
        // Check if the target has indeed been found.
        // It was previously possible for two sections of the navmesh to appear coherent but in reality be disjointed.
        // This made it so while technically containing both the starting and ending point, a path connecting them is impossible.
        if (searched.find(targetTriangle) != searched.end())
        {   
            path.push_back(current);

            // The path is only done once we have reached the starting point again.
            while(searched[current] > 0) 
            {
                // Initialise bestNeighbour index and a check on distance.
                int bestNeighbour = -1;
                int minDistance = searched[current];

                for (const unsigned int& neighbour : meshIndexes[current].neighbors) 
                {
                    // Ensure every checked neighbour actually exists.
                    // CDT always has 3 neighbours for a triangle, either their index or noNeighbor value.
                    if (neighbour == CDT::noNeighbor) { continue; }

                    // Find the actual minimal distance to go.
                    // We could simply decrement by one every time, but this way slightly optimises winding pathways.
                    if (searched.find(neighbour) != searched.end() && searched[neighbour] < minDistance) 
                    {
                        minDistance = searched[neighbour];
                        bestNeighbour = neighbour;
                    }
                }

                // Check if something went wrong leading to the current triangle not having any valid neighbours.
                if (bestNeighbour == -1) {break;}

                // Add the best choice for the next node on the path.
                path.push_back(bestNeighbour);
                // Then start back up from that one.
                current = bestNeighbour;
            }

            // Reverse the path vector again to set it to be correctly front to back.
            std::reverse(path.begin(), path.end());


            // This section is TEMPORARY and for TESTING only, 
            // it creates the draweable path aswell.
            for (size_t i = 0; i < path.size() - 1; i++)
            {
                std::vector<Vector2> triangle1 = GetTrianglePoints(meshIndexes[path[i]], meshPoints);
                std::vector<Vector2> triangle2 = GetTrianglePoints(meshIndexes[path[i+1]], meshPoints);
                Vector2 pos1 = GetTriangleMiddlePoint(triangle1);
                Vector2 pos2 = GetTriangleMiddlePoint(triangle2);
                linePath.push_back(Line{pos1, pos2, BLUE});
            }
        }
    }
};

// Agent update function allowing it to follow a designated path.
void PathFinder::Update(const std::vector<CDT::Triangle>& meshIndexes,const std::vector<CDT::V2d<double>>& meshPoints)
{
    // If the path is empty, it simply means the agent has no active target destination, and should remain stationary.
    if (!path.empty()) 
    {
        // Set up the goal triangle as the next triangle on the path.
        std::vector<Vector2> goalTriangle = GetTrianglePoints(meshIndexes[path[0]], meshPoints);
        // Get it's position directly to simplify position checks.
        Vector2 goal = GetTriangleMiddlePoint(goalTriangle);

        // If the goal is very close to the current position, we can consider it already reached.
        if (std::abs(goal.x - position.x) < 1 && std::abs(goal.y - position.y) < 1) 
        {
            // We can therefore erase it from the path and follow the next point in the path on the next call of Update.
            path.erase(path.begin());
            if (!linePath.empty()) { linePath.erase(linePath.begin()); }
            return;
        }

        // If the agent is not at the current goal point, it moves towards it.
        speed = 2.0f;
        float direction = atan2(goal.y - position.y, goal.x - position.x);

        float deltaX = std::cos(direction) * speed;
        float deltaY = std::sin(direction) * speed;

        position.x += deltaX;
        position.y += deltaY;
    }
};

// Simple draw function for the actual path the agent takes.
void PathFinder::DrawPath() const 
{
    if (!linePath.empty()) { for (const Line& line : linePath) { line.Draw(); } }
};
