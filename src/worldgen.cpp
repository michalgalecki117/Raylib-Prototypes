#include "worldgen.h"
#include <random>

//-------------------------------------------------------------------------------------------------------------------------------
// Perlin Class. 
//-------------------------------------------------------------------------------------------------------------------------------

// Constructor with variables.
// Variables set by default to allow for default constructor usage.
Perlin::Perlin(uint32_t seed, double scale, double persistence) 
    : m_perlin(seed), seed(seed), scale(scale), persistence(persistence) 
    {
        version = 1;
    }

// Simple Getters and Setters.
void Perlin::setSeed(uint32_t newSeed)
{
    seed = newSeed;
    m_perlin = siv::PerlinNoise(seed);
    version++;
} // Reseeds the noise to fully reset it.

void Perlin::setScale(double newScale)
{ 
    scale = newScale;
    version++;
}

void Perlin::setPersistence(double newPersistence) 
{
    persistence = newPersistence; 
    version++;
}

// Simple 2D sampling function.
double Perlin::sampleNoise(int x, int y) const { return m_perlin.normalizedOctave2D_01(x * scale, y * scale, 4, persistence); }

//-------------------------------------------------------------------------------------------------------------------------------
// World Class. 
//-------------------------------------------------------------------------------------------------------------------------------

World::World(const Perlin& perlin, float threshold, std::vector<Obstacle> obstacles )
    : perlin(perlin), threshold(threshold), obstacles(std::move(obstacles))
{
    version = 1;
}

World World::makeWorld(const Perlin& perlin, float threshold, std::vector<Obstacle> obstacles) 
{
    return World(perlin, threshold, std::move(obstacles));
}

World World::makeWorld(const Perlin& perlin, float threshold, Obstacle obstacle) 
{
    std::vector<Obstacle> obstacles;
    obstacles.push_back(std::move(obstacle));
    return makeWorld(perlin, threshold, std::move(obstacles));
}

World World::makeWorld(const Perlin& perlin, float threshold) 
{
    return makeWorld(perlin, threshold, {});
}

void World::setThreshold(float newThreshold) 
{
    threshold = newThreshold;
    version++;
}

//-------------------------------------------------------------------------------------------------------------------------------
// WorldGen Class. 
//-------------------------------------------------------------------------------------------------------------------------------

WorldGen::WorldGen(uint32_t seed, double scale, double persistence, float threshold)
    : perlin(Perlin(seed, scale, persistence)),
    world(World::makeWorld(this->perlin, threshold))
{}

WorldGen WorldGen::makeNew(uint32_t seed, double scale, double persistence, float threshold)
{
    return WorldGen(seed, scale, persistence, threshold);
}

void WorldGen::reset(uint32_t seed, double scale, double persistence, float threshold) 
{
    perlin.setSeed(seed);
    perlin.setScale(scale);
    perlin.setPersistence(persistence);
    world.setThreshold(threshold);
}