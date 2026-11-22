#pragma once

#include <vector>
#include <map>
#include <utility>

#include "PerlinNoise.hpp"
#include "CDT/CDT.h"

#include "obstacle.h"

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
    
    // World edges.
    std::vector<std::pair<Vector2, Vector2>> edges;
    // Value threshold for noise, for which edge-detection activates.
    float threshold;
    // All obstacles in world.
    std::vector<Obstacle> obstacles;
    // Version for pipeline control.
    int version;

    // Private constructor for safety.
    World(const Perlin& perlin, float threshold, std::vector<Obstacle> obstacles );

public:
    // Public factories.
    static World makeWorld(const Perlin& perlin, float threshold);
    static World makeWorld(const Perlin& perlin, float threshold, Obstacle obstacle);
    static World makeWorld(const Perlin& perlin, float threshold, std::vector<Obstacle> obstacles);

    // Simple Getters and Setters.
    float getThreshold() const { return threshold; }
    void setThreshold(float newThreshold);

    int getVersion() const { return version; }
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
    WorldGen(uint32_t seed, double scale, double persistence, float threshold);

public:
    // Public factories.
    static WorldGen makeNew(uint32_t seed, double scale, double persistence, float threshold);

    // Refresh function to empty worldgen without destroying the object itself.
    void reset(uint32_t seed, double scale, double persistence, float threshold);

    // Simple Getters and Setters.
    const Perlin& getPerlin() const { return perlin; }
    const World& getWorld() const { return world; }
};