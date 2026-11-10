#pragma once
#include <vector>
#include <raylib.h>

#include "CDT/CDT.h"
#include "geometry.h"

class PathFinder : public Circle // Agent capable of path finding.
{
public:
    // Constructor with variables.
    // There is no default constructor as agent position will depend on available space within the navmesh.
    PathFinder(Vector2 startPos, float startRad, Color startColor, float startSpeed) 
        : Circle(startPos, startRad, startColor)
        , speed(startSpeed)
    {}

    // Complex function generating a path from the navmesh, given a target position.
    void GeneratePath(Vector2 targetPosition,const std::vector<CDT::Triangle>& meshIndexes,const std::vector<CDT::V2d<double>>& meshPoints);

    // Update function to allow the agent to follow a designated path.
    void Update(const std::vector<CDT::Triangle>& meshIndexes,const std::vector<CDT::V2d<double>>& meshPoints);

    // Simple drawing function.
    void DrawPath() const;

private:
    std::vector<Line> linePath;
    std::vector<int> path;
    float speed;

    std::vector<Vector2> GetTrianglePoints(const CDT::Triangle& triangle, const std::vector<CDT::V2d<double>> meshPoints);
    Vector2 GetTriangleMiddlePoint(const std::vector<Vector2>& triangle);
};