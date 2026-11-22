#pragma once

#include <vector>
#include <map>
#include <utility>
#include <cstdint>
#include <cmath>
#include <algorithm>

#include "PerlinNoise.hpp"
#include "CDT/CDT.h"

#include "obstacle.h"
#include "shapes.h"

class Perlin // Perlin noise class for generating a perlin noise texture, or noise sampling.
{
private:
    // Generated perlin noise set itself.
    siv::PerlinNoise m_perlin;
    // Parameters for generation.
    uint32_t seed;
    double scale;
    double persistence;
    // Version number for pipeline control.
    int version;

    public:
    // Constructor with variables.
    // Variables set by default to allow for default constructor to function.
    Perlin(uint32_t seed = 1337, double scale = 0.015, double persistence = 0.5);
    ~Perlin() = default; // Default destructor.
    
    // Simple Getters and Setters.
    uint32_t getSeed() const { return seed; }
    void setSeed(uint32_t newSeed); // Reseeds the perlin noise.

    double getScale() const { return scale; }
    void setScale(double newScale);

    double getPersistence() const {return persistence; } 
    void setPersistence(double newPersistence);

    int getVersion() const { return version; }

    // Simple 2D sampling function.
    double sampleNoise(int x, int y) const;
};

class World // Creates and stores world/object edges. 
{
private:
    // Reference to perlin noise from which the world is made.
    const Perlin& perlin;
    // World edges as vector of points and pairs of point indexes.
    std::vector<Vector2> points;
    std::vector<std::pair<size_t, size_t>> edges;
    // Value threshold for noise, for which edge-detection activates.
    float threshold;
    // All obstacles in world.
    std::vector<Obstacle> obstacles;
    // Dimensions of world, this determines the absolute cut-off point.
    Vector2 dimensions;
    // Size of marching squares cells, required for edge-detection.
    int cellSize;
    // Version for pipeline control.
    int version;

    // Private constructor for safety.
    World(const Perlin& perlin, float threshold, std::vector<Obstacle> obstacles, Vector2 dimensions, int cellSize);
    
    // Private function for interpolating points based on a given threshold.
    // Basically it finds more accurate points to connect the vertices to rather than just running them through the corners themselves.
    // This allows our lines to have different angles.
    // Also snaps points to the border if intersecting.
    Vector2 interpolatePoints(Vector2 p1, Vector2 p2, double v1, double v2, Vector2 dimensions) const;

    // This function is used to build a segmented line across an axis.
    // It starts in cornerA and ends in cornerB, making sure to create a line across every pair of points in pts vector.
    // To do so, it must first sort all the points along either the X or Y axis.
    void buildBorderSegments(std::vector<Vector2>& pts, Vector2 cornerA, Vector2 cornerB, bool sortX);

    // This is a small helper function for marching squares.
    // It is used to check if a point is already in the points vector,
    // and if not add it. It always returns the point's new index.
    size_t addUniquePoint(Vector2 point, float epsilon);
    bool isInsideBorder(Vector2 pt, float minP, float maxX, float maxY);

public:
    // Public factories.
    static World makeWorld(const Perlin& perlin, float threshold, Vector2 dimensions, int cellSize = 15);
    static World makeWorld(const Perlin& perlin, float threshold, Obstacle obstacle, Vector2 dimensions, int cellSize = 15);
    static World makeWorld(const Perlin& perlin, float threshold, std::vector<Obstacle> obstacles, Vector2 dimensions, int cellSize = 15);

    // Simple Getters and Setters.
    float getThreshold() const { return threshold; }
    void setThreshold(float newThreshold);

    Vector2 getDimensions() const { return dimensions; }
    int getCellSize() const { return cellSize; }
    int getVersion() const { return version; }

    std::vector<Vector2> getUniquePoints() const { return points; }
    std::vector<std::pair<size_t, size_t>> getEdges() const { return edges; }

    // Marching squares edge-detection.
    void generateEdgesFromNoise();
};


class Navmesh // Creates and stores world navmesh.
{
private :
    // Vector to store our navmesh triangles in.
    std::vector<CDT::Triangle> triangles;
    // And also the set of triangle corners for later navigation.
    std::vector<CDT::V2d<double>> points;
    // Private constructor for safety of data.
    Navmesh(const World& world);

public :
    // Public factories.
    static Navmesh makeNavmesh(const World& world);
};


class WorldGen // Controller class for world generation pipeline.
{
private:
    // Holds and owns the base noise used to generate world.
    Perlin perlin;
    // Holds the generated world itself.
    World world;
    // Holds all variants of navmeshed generated from the world.
    std::map<float, Navmesh> meshes;

    // Private constructor for safety.
    WorldGen(uint32_t seed, double scale, double persistence, float threshold, Vector2 dimensions, int cellSize);

public:
    // Public factories.
    static WorldGen makeNew(uint32_t seed, double scale, double persistence, float threshold, Vector2 dimensions, int cellSize = 15);

    // Refresh function to empty worldgen without destroying the object itself.
    void reset(uint32_t seed, double scale, double persistence, float threshold);

    // Simple Getters and Setters.
    const Perlin& getPerlin() const { return perlin; }
    const World& getWorld() const { return world; }
};