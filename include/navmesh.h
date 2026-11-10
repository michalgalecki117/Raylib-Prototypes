#pragma once
#include <raylib.h>
#include <vector>

#include "geometry.h"
#include "perlin.h"
#include "CDT/CDT.h"

class World : public GameObject, public Perlin // World object class to contain all collision vertices.
{
public:
    // Constructor with variables.
    World(
        Vector2 dimensions, uint32_t seed = 1337, double scale = 0.01, 
        double persistence = 0.25, double threshold = 0.5, int cellSize = 15
    ); // Variables set by default to allow for default constructor usage.


    // Simple Getters and Setters.
    double GetThreshold() const;
    void SetThreshold(double newThreshold); 

    std::vector<Line> GetVertices() const { return lines; }    

    // Marching Squares algorithm to generate map vertices based on perlin noise.
    void GenerateVertices(Vector2 dimensions);
    
    // Refresh function to remake the perlin noise and recalculate vertices.
    Texture2D Refresh(Vector2 dimensions);
    

    // Simple update and draw functions.
    void update();
    void Draw() const override;
    void DrawGrid() const;
    
private:
    // Inherited position and perlin noise of the world.
    // Array containing all collision vertices of the world map.
    // Used for direct collision and drawing.
    std::vector<Line> lines;
    // Threshold on which vertices are calculated.
    double threshold;
    // CellSize for the Marching Squares algorithm.
    int cellSize;

    // Small vector of lines to store the grid, purely for visualisation of marching squares algorithm.
    std::vector<Line> grid;

    // Private functions to simplify the Marching Squares edge finding algorithm.
    
    // Interp interpolates between two points on an edge based on their values in perlin, also snaps points to border if needed.
    Vector2 Interp(Vector2 p1, Vector2 p2, double v1, double v2, Vector2 dimensions) const;
    // This function builds out a segment of edges along an axis, from cornerA to cornerB and passing through every point in pts.
    void BuildBorderSegments(std::vector<Vector2>& pts, Vector2 cornerA, Vector2 cornerB, bool sortX);
};



class Navmesh 
{
public :
    // Constructor with variables. No default constructor needed.
    Navmesh(const std::vector<Line> lines);

    // Simple Getters and Setters.
    std::vector<CDT::Triangle> GetTriangles() const { return triangles; };
    std::vector<CDT::V2d<double>> GetPoints() const { return points; };

    // Simple Drawing function.
    void DrawPoints() const;
    void DrawNavmesh() const;

private :
    // Vector to store our navmesh triangles in.
    std::vector<CDT::Triangle> triangles;
    // And also the set of triangle corners for later navigation.
    std::vector<CDT::V2d<double>> points;
    // Point storage vector for visualisation only.
    std::vector<Vector2> uniquePoints;
};