#include "worldgen.h"
#include <random>

#include <fstream>

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

// Private constructor. Sets up the perlin reference, the obstacles vector aswell as the variables used for edge-detection.
World::World(const Perlin& perlin, float threshold, std::vector<Obstacle> obstacles, Vector2 dimensions, int cellSize)
    : perlin(perlin), threshold(threshold), obstacles(std::move(obstacles)), dimensions(dimensions), cellSize(cellSize)
{
    // Initialize version and generate edges.
    version = 1;
    generateEdgesFromNoise();
}

// Public factories. 
// Public factory of world, uses a premade vector of obstacles.
World World::makeWorld(const Perlin& perlin, float threshold, std::vector<Obstacle> obstacles, Vector2 dimensions, int cellSize) 
{
    return World(perlin, threshold, std::move(obstacles), dimensions, cellSize);
}

// Public factory of world, uses a single obstacle object at initialisation.
World World::makeWorld(const Perlin& perlin, float threshold, Obstacle obstacle, Vector2 dimensions, int cellSize) 
{
    std::vector<Obstacle> obstacles;
    obstacles.push_back(std::move(obstacle));
    return makeWorld(perlin, threshold, std::move(obstacles), dimensions, cellSize);
}

// Public factory of world, no objects are required for initialising World object.
World World::makeWorld(const Perlin& perlin, float threshold, Vector2 dimensions, int cellSize) 
{
    return makeWorld(perlin, threshold, {}, dimensions, cellSize);
}

// Simple Getters and Setters.
void World::setThreshold(float newThreshold) 
{
    threshold = newThreshold;
    version++;
}

// Private function for interpolating points based on a given threshold.
// Basically it finds more accurate points to connect the vertices to rather than just running them through the corners themselves.
// This allows our lines to have different angles.
// Also snaps points to the border if intersecting.
Vector2 World::interpolatePoints(Vector2 p1, Vector2 p2, double v1, double v2, Vector2 dimensions) const 
{
    // Choose one of the points if both are superimposed over each other.
    if (std::abs(v1 - v2) < 0.00001f) return p1;

    // Calculate the fractional distance between the two points and the threshold.
    double t = (threshold - v1) / (v2 - v1);
    
    // Clamp to [0.0, 1.0] to avoid out of bounds points.
    if (t < 0) { t = 0.0f;}
    if (t > 1) {t = 1.0f;}

    // Create the new point created along the p1, p2 edge based on the fractional distance using threshold.
    float rawX  = static_cast<float>(p1.x + t * (p2.x - p1.x));
    float rawY = static_cast<float>(p1.y + t * (p2.y - p1.y));
    
    // Round said point to the nearest pixel to avoid errors in later use, then return.
    float roundedX = round(rawX);
    float roundedY = round(rawY);

    return {roundedX, roundedY};
};

bool World::isInsideBorder(Vector2 pt, float minP, float maxX, float maxY)
{
    return pt.x >= minP && pt.x <= maxX &&
           pt.y >= minP && pt.y <= maxY;
}

// This function is used to build a segmented line across an axis.
// It starts in cornerA and ends in cornerB, making sure to create a line across every pair of points in pts vector.
// To do so, it must first sort all the points along either the X or Y axis.
void World::buildBorderSegments(std::vector<Vector2>& pts, Vector2 cornerA, Vector2 cornerB, bool sortX)
{
    // Indexes of two points of a tested edge, to add to edges vector.
    size_t index1;
    size_t index2;

    // Epsilon to equate points to each other based off of cellSize.
    float epsilon = static_cast<float>(cellSize) / 20.0f;

    // Manually add in both selected corners to the pts vector.
    // But only if the corners are indeed in-bounds.
    if (perlin.sampleNoise(cornerA.x, cornerA.y) >= threshold) { pts.push_back(cornerA); }
    if (perlin.sampleNoise(cornerB.x, cornerB.y) >= threshold) { pts.push_back(cornerB); }

    // Sort points along the chosen axis (X or Y).
    sort(pts.begin(), pts.end(), [sortX](const Vector2& a, const Vector2& b)
    {
        return sortX ? (a.x < b.x) : (a.y < b.y);
    });

    // Remove duplicate points (if multiple points connect in the same place at the border).
    pts.erase(
        // Small lambda function to get the unique points.
        unique(pts.begin(), pts.end(), [](const Vector2& a, const Vector2& b)
        {   
            return abs(a.x - b.x) < 0.1f && abs(a.y - b.y) < 0.1f;
        }),
        pts.end()
    );

    // Create consecutive edge segments between every pair of points.
    for (size_t i = 0; i + 1 < pts.size(); i++)
    {
        // Calculate the distance between them.
        // Since border points either on X or Y axis, this distance is simply the difference.
        float distance = sortX ? (pts[i + 1].x - pts[i].x) : (pts[i + 1].y - pts[i].y);
        // Pass in case of coinciding points.
        if (distance <= 0.0f)
        {
            continue;
        }
        // Calculate the amount of intermiediate points to check for value.
        // This ensures with reasonable error rates that the checked segment is indeed,
        // inside or outside of bounds.
        int steps = static_cast<int>(distance) / cellSize;
        // Ensure at least one point is checked, even if the segment is too small.
        if (steps < 1)
        {
            steps = 1;
        }

        // Distance between check points. Ensure neither of the actual segment ends are checked.
        // They obviously pass the test by default since they're being checked at all.
        float delta = distance / static_cast<float>(steps + 1);
        // Reset count to 0 for every pair.
        int count = 0;
        // Test check points strictly between pts[i] and pts[i + 1].
        for (int s = 1; s <= steps; s++)
        {
            // Calculate the exact positions of the check points.
            float offset = delta * static_cast<float>(s);
            float sampleX = sortX ? (pts[i].x + offset) : pts[i].x;
            float sampleY = sortX ? pts[i].y : (pts[i].y + offset);

            // Count the amount of check points passing threshold value.
            if (perlin.sampleNoise(static_cast<int>(sampleX), static_cast<int>(sampleY)) >= threshold)
            {
                count++;
            }
        }

        // Add segment if at least half of sampled interior points pass the threshold.
        // This value can be modified to ensure stricter control.
        // This might cause errors, modify at your own risk.
        if (count * 2 >= steps)
        {
            // Add points only if they're unique in points vector.
            index1 = addUniquePoint(pts[i], epsilon);
            index2 = addUniquePoint(pts[i + 1], epsilon);
            // And so long as they're not the same point...
            if (index1 != index2)
            {
                // Add their indexes back into edges vector.
                edges.push_back({index1, index2});
            }
        }
    }
}

// This is a small helper function for marching squares.
// It is used to check if a point is already in the points vector,
// and if not add it. It always returns the point's new index.
size_t World::addUniquePoint(Vector2 point, float epsilon) 
{
    // Loop through all existing points.
    for (size_t i = 0; i < points.size(); i++) 
    {
        // If points too close together. Treat them as the same point and return the existing point's index.
        if (std::abs(points[i].x - point.x) < epsilon && std::abs(points[i].y - point.y) < epsilon) 
        {
            return i;
        }
    }
    // If not, add the new point and return it's index.
    points.push_back(point);
    return points.size() - 1;
}

/*
Marching squares divides the entire space into a grid of cells the size of cellSize.
Then each corner is sampled from perlin noise and checked against the specified threshold,
With the whole cell checked, the line can be decided depending on correct corners.
This loops through the whole noise of given dimensions to produce an array of all interconnecting edges.
As long  as  it works, keep it ;)
*/
void World::generateEdgesFromNoise() 
{
    // Increment version of world edges on every call of this function.
    // It is only ever recalculated if underlying perlin noise changed.
    version++;

    // Destroys the edges vector before generating a new one.
    points.clear();
    edges.clear();

    // Indexes of two points of a tested edge, to add to edges vector.
    size_t index1;
    size_t index2;
    // Epsilon to equate points to each other based off of cellSize.
    float epsilon = cellSize / 20;

    // Define the four corners of the world;
    float minP = static_cast<float>(cellSize);
    float maxX = static_cast<float>(((int)dimensions.x / cellSize) * cellSize);
    float maxY = static_cast<float>(((int)dimensions.y / cellSize) * cellSize);

    Vector2 topLeft     = { minP, minP };
    Vector2 topRight    = { maxX, minP };
    Vector2 bottomLeft  = { minP, maxY };
    Vector2 bottomRight = { maxX, maxY };

    // This is the intermediate vector holding cellLines for each cell.
    std::vector<std::pair<Vector2, Vector2>> cellLines;

    // This is a set of vectors for each side of the border.
    // They are used to store the coordinates of every point that got snapped to it,
    // so every point along a line that would have intersected with the border otherwise.
    std::vector<Vector2> TopIntersects;
    std::vector<Vector2> RightIntersects;
    std::vector<Vector2> BottomIntersects;
    std::vector<Vector2> LeftIntersects;

    // Go through every cell in the grid.
    // Make sure to stop one further than needed. (dimension - cellSize).
    // This ensures complete map coverage even if cellSize doesn't divide dimensions properly.
    for (int y = cellSize; y < static_cast<int>(maxY); y += cellSize) {
        for (int x = cellSize; x < static_cast<int>(maxX); x += cellSize) {
            
            // Clear intermediate variable array.
            cellLines.clear();

            // Calculate the coordinates for the 4 corners of every cell.
            Vector2 tl = { (float)x, (float)y };
            Vector2 tr = { (float)(x + cellSize), (float)y };
            Vector2 br = { (float)(x + cellSize), (float)(y + cellSize) };
            Vector2 bl = { (float)x, (float)(y + cellSize) };

            // Sample the values for each corner based on our perlin noise.
            double vTL = perlin.sampleNoise(x, y);
            double vTR = perlin.sampleNoise(x + cellSize, y);
            double vBR = perlin.sampleNoise(x + cellSize, y + cellSize);
            double vBL = perlin.sampleNoise(x, y + cellSize);

            // Create a seperate case for all combinations of all corners passing the threshold. (or not)
            // This uses binary OR instead of simple addition to render it more efficient.
            int index = 0;
            if (vTL >= threshold) index |= 1;
            if (vTR >= threshold) index |= 2;
            if (vBR >= threshold) index |= 4;
            if (vBL >= threshold) index |= 8;

            // Minor optimisation, but if the index is 0 or 15, 
            // it means the entire cell is in or out of a figure and no lines will be drawn across it.
            if (index == 0 || index == 15) continue;

            // Decide the best points along each edge to draw the lines from/to based on interpolation.
            Vector2 eTop = interpolatePoints(tl, tr, vTL, vTR, dimensions);
            Vector2 eRight = interpolatePoints(tr, br, vTR, vBR, dimensions);
            Vector2 eBottom = interpolatePoints(bl, br, vBL, vBR, dimensions);
            Vector2 eLeft = interpolatePoints(tl, bl, vTL, vBL, dimensions);

            // Choose the correct edges depending on previously decided cases.
            // Include the edge cases of saddle cells requiring two lines instead of one.
            switch (index) {
                case 1:  cellLines.push_back({eLeft, eTop}); break;
                case 2:  cellLines.push_back({eTop, eRight}); break;
                case 3:  cellLines.push_back({eLeft, eRight}); break;
                case 4:  cellLines.push_back({eRight, eBottom}); break;
                case 5:  cellLines.push_back({eLeft, eTop}); cellLines.push_back({eRight, eBottom}); break;
                case 6:  cellLines.push_back({eTop, eBottom}); break;
                case 7:  cellLines.push_back({eLeft, eBottom}); break;
                case 8:  cellLines.push_back({eLeft, eBottom}); break;
                case 9:  cellLines.push_back({eTop, eBottom}); break;
                case 10: cellLines.push_back({eTop, eLeft}); cellLines.push_back({eBottom, eRight}); break;
                case 11: cellLines.push_back({eRight, eBottom}); break;
                case 12: cellLines.push_back({eLeft, eRight}); break;
                case 13: cellLines.push_back({eTop, eRight}); break;
                case 14: cellLines.push_back({eLeft, eTop}); break;
            }

            // Create new edges in the edges vector based on our edge coordinate pairings.
            for (auto &pair : cellLines) {

                if (!isInsideBorder(pair.first, minP, maxX, maxY) || !isInsideBorder(pair.second, minP, maxX, maxY)) { continue; }

                index1 = addUniquePoint(pair.first, epsilon);
                index2 = addUniquePoint(pair.second, epsilon);
                
                if (index1 != index2) { edges.push_back({index1 , index2}); }
                
                // Note the point coordinates if it was snapped to the border.
                // This is done to create border edges to enclose outer holes.
                if (abs(pair.first.y - cellSize) < 0.1f)                        { TopIntersects.push_back(pair.first); }
                if (abs(pair.first.x - cellSize) < 0.1f)                        { LeftIntersects.push_back(pair.first); }
                if (abs(pair.first.y - (maxY)) < 0.1f)       { BottomIntersects.push_back(pair.first); }
                if (abs(pair.first.x - (maxX)) < 0.1f)       { RightIntersects.push_back(pair.first); }

                if (abs(pair.second.y - cellSize) < 0.1f)                        { TopIntersects.push_back(pair.second); }
                if (abs(pair.second.x - cellSize) < 0.1f)                        { LeftIntersects.push_back(pair.second); }
                if (abs(pair.second.y - (maxY)) < 0.1f)       { BottomIntersects.push_back(pair.second); }
                if (abs(pair.second.x - (maxX)) < 0.1f)       { RightIntersects.push_back(pair.second); }
            }
        }
    }

    // Build segmented lines for each of the 4 borders using our member function
    buildBorderSegments(TopIntersects, topLeft, topRight, true);       // Top: sorted by X
    buildBorderSegments(RightIntersects, topRight, bottomRight, false);  // Right: sorted by Y
    buildBorderSegments(BottomIntersects, bottomLeft, bottomRight, true); // Bottom: sorted by X
    buildBorderSegments(LeftIntersects, topLeft, bottomLeft, false);     // Left: sorted by Y
}

//-------------------------------------------------------------------------------------------------------------------------------
// WorldGen Class. 
//-------------------------------------------------------------------------------------------------------------------------------

// Private WorldGen constructor, requires the variables for perlin and world setups.
WorldGen::WorldGen(uint32_t seed, double scale, double persistence, float threshold, Vector2 dimensions, int cellSize)
    : perlin(Perlin(seed, scale, persistence)),
    world(World::makeWorld(this->perlin, threshold, dimensions, cellSize))
{}

// Public WorldGen factory.
WorldGen WorldGen::makeNew(uint32_t seed, double scale, double persistence, float threshold, Vector2 dimensions, int cellSize)
{
    return WorldGen(seed, scale, persistence, threshold, dimensions, cellSize);
}

// WorldGen reset function to call to completely rebuild all owned objects from scratch.
// To be used sparingly, as it will force the recalculation of all edges, obstacles and meshes.
// Ensures all parts of world generation pipeline are recalculated in proper order and way.
void WorldGen::reset(uint32_t seed, double scale, double persistence, float threshold) 
{
    // Resetting perlin noise.
    perlin.setSeed(seed);
    perlin.setScale(scale);
    perlin.setPersistence(persistence);
    
    // Resetting world edges.
    world.setThreshold(threshold);
    world.generateEdgesFromNoise();
}