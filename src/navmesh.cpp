#include "navmesh.h"
#include <raylib.h>
#include <vector>
#include <cmath>

// Constructor with variables.
// Variables set by default to allow default constructor usage.
World::World(Vector2 dimensions, uint32_t seed, double scale, double persistence, double threshold, int cellSize)
    : Perlin(seed, scale, persistence), threshold(threshold), cellSize(cellSize) 
{
    GenerateVertices(dimensions);
} // Vertices array generated on initialisation.


// Simple Getters and Setters.
double World::GetThreshold() const { return threshold; }
void World::SetThreshold(double newThreshold) { threshold = newThreshold; } 


// 

// Marching Squares algorith generating vertices based on threshhold and perlin noise.
void World::GenerateVertices(Vector2 dimensions)
/*
Marching squares divides the entire space into a grid of cells the size of CellSize.
Then each corner is sampled from perlin noise and checked against the specified threshold,
With the whole cell checked, the line can be decided depending on correct corners.
This loops through the whole noise of given dimensions to produce an array of all interconnecting edges.
*/
{
    // Destroys the lines array before generating a new one.
    lines.clear();
    
    // Seperating given dimensions to simplify further math.
    int width = static_cast<int>(dimensions.x);
    int height = static_cast<int>(dimensions.y);

    // This is the intermediate array holding cellLines for each cell.
    std::vector<std::pair<Vector2, Vector2>> cellLines;

    // interp is the small lambda function unique to marching squares.
    // It interpolates two points using their values and a given threshold.
    auto interp = [&](Vector2 p1, Vector2 p2, double v1, double v2)
    {
        // Choose one of the points if both are superimposed over each other.
        if (std::abs(v1 - v2) < 0.00001f) return p1;
        // Calculate the fractional distance between the two points and the threshold.
        double t = (threshold - v1) / (v2 - v1);
        // Return the new point created along the p1, p2 edge based on the fractional distance using threshold.
        return (Vector2){ static_cast<float>(p1.x + t * (p2.x - p1.x)), static_cast<float>(p1.y + t * (p2.y - p1.y)) };
    };
    // Basically it finds more accurate points to connect the vertices to rather than just running them through the corners themselves.
    // This allows our lines to have different angles.

    // Go through every cell in the grid.
    for (int y = 0; y < height - cellSize; y += cellSize) {
        for (int x = 0; x < width - cellSize; x += cellSize) {
            
            // Clear intermediate variable array.
            cellLines.clear();

            // Calculate the coordinates for the 4 corners of every cell.
            Vector2 tl = { (float)x, (float)y };
            Vector2 tr = { (float)(x + cellSize), (float)y };
            Vector2 br = { (float)(x + cellSize), (float)(y + cellSize) };
            Vector2 bl = { (float)x, (float)(y + cellSize) };

            // Sample the values for each corner based on our perlin noise.
            double vTL = SampleNoise(x, y);
            double vTR = SampleNoise(x + cellSize, y);
            double vBR = SampleNoise(x + cellSize, y + cellSize);
            double vBL = SampleNoise(x, y + cellSize);

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
            Vector2 eTop = interp(tl, tr, vTL, vTR);
            Vector2 eRight = interp(tr, br, vTR, vBR);
            Vector2 eBottom = interp(bl, br, vBL, vBR);
            Vector2 eLeft = interp(tl, bl, vTL, vBL);

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

            // Create new vertices in the vertices array based on our edge coordinate pairings.
            for (auto &pair : cellLines) {
                lines.push_back(Line(pair.first, pair.second, RED));
            }
        }
    }
}

// Refresh function to remake the perlin noise and recalculate vertices.
Texture2D World::Refresh(Vector2 dimensions) 
// Used in case of perlin objects variables changing.
{
    // Regenerate a new Vertices array using the new perlin noise.
    GenerateVertices(dimensions);
    // Also update generated texture for visualisation.
    return GenerateTexture(static_cast<int>(dimensions.x), static_cast<int>(dimensions.y), threshold);
}


void World::update() {} // Potential update function for later use.

// Simple draw function.
void World::Draw() const 
{
    // Calls the draw function of every line in the Vertices array.
    for (const Line &line : lines) 
    {
        line.Draw();
    }
}