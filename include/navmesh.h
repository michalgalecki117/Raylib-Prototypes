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
    
private:
    // Inherited position and perlin noise of the world.
    // Array containing all collision vertices of the world map.
    // Used for direct collision and drawing.
    std::vector<Line> lines;
    // Threshold on which vertices are calculated.
    double threshold;
    // CellSize for the Marching Squares algorithm.
    int cellSize;
};



class Navmesh 
{
public :
    // Constructor with variables. No default constructor needed.
    Navmesh(const std::vector<Line> lines);

    // Simple Drawing function.
    void Draw();

private :
    // Vector to store our navmesh triangles in.
    std::vector<CDT::Triangle> triangles;
    // And also the set of triangle edges for later navigation.
    std::vector<CDT::V2d<double>> vertices;

    // TEMPORARY POINT STORAGE FOR PROGRESS DISSPLAY
    std::vector<Vector2> uniquePoints;

};