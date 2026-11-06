#include "navmesh.h"
#include <raylib.h>
#include <vector>
#include <cmath>
#include <unordered_map>


//-------------------------------------------------------------------------------------
// World class.
//-------------------------------------------------------------------------------------

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


// Marching Squares algorith generating vertices based on threshhold and perlin noise.
void World::GenerateVertices(Vector2 dimensions)
/*
Marching squares divides the entire space into a grid of cells the size of CellSize.
Then each corner is sampled from perlin noise and checked against the specified threshold,
With the whole cell checked, the line can be decided depending on correct corners.
This loops through the whole noise of given dimensions to produce an array of all interconnecting edges.
As long  as  it works, keep it ;)
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
        // Create the new point created along the p1, p2 edge based on the fractional distance using threshold.
        float rawX  = static_cast<float>(p1.x + t * (p2.x - p1.x));
        float rawY = static_cast<float>(p1.y + t * (p2.y - p1.y));
        // Round said point to the nearest pixel to avoid errors in later use, then return.
        float roundedX = round(rawX);
        float roundedY = round(rawY); 
        return (Vector2){roundedX, roundedY};
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

    lines.push_back(Line({(float)cellSize, (float)cellSize},{(float)(dimensions.x - cellSize),(float)cellSize},PURPLE));
    lines.push_back(Line({(float)(dimensions.x - cellSize), (float)cellSize},{(float)(dimensions.x - cellSize),(float)(dimensions.y - cellSize)},PURPLE));
    lines.push_back(Line({(float)cellSize, (float)cellSize},{(float)cellSize,(float)(dimensions.y - cellSize)},PURPLE));
    lines.push_back(Line({(float)cellSize, (float)(dimensions.y - cellSize)},{(float)(dimensions.x - cellSize),(float)(dimensions.y - cellSize)},PURPLE));

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
};

//-------------------------------------------------------------------------------------
// Navmesh class.
//-------------------------------------------------------------------------------------


// This is a custom written Functor. (A type acting as a function)
// We define it this way because unordered_map only accepts types and not functions directly.
// It is used to redifine how the hash is calculated for each point. Given no default hash for Vector2.
struct Vector2Hash
{
    // We surcharge the () operator so the Vector2Hash type can work as a function.
    // We pass it the vector in const by reference as a minor optimisation for memory access.
    // It also returns a size_t type which is simply unsigned long long, it is the default index type of unordered_map.
    size_t operator()(const Vector2& v) const 
    {
        // The hash function is known for simple float values, but needs to be calculated for both members of the vector.
        // {} is used too instantiate the hash function using v.x and v.y as variables respectively.
        size_t h1 = std::hash<float>{}(v.x);
        size_t h2 = std::hash<float>{}(v.y);
        // And then later combined.
        return h1 ^ (h2 << 1);
    }
};

// This is a custom written Functor. (A type acting as a function)
// We define it this way because unordered_map only accepts types and not functions directly.
// It is used to redifine the equals operation between two vector points.
// In case there is a hash equality between points, we reverify they are indeed the same or not the same point.
struct Vector2Equals
{
    // We surcharge the () operator so the Vector2Equal type can work as a function.
    // We pass it the vector in const by reference as a minor optimisation for memory access.
    bool operator()(const Vector2& v1, const Vector2& v2) const 
    {
        // Check if the coordinates of given points match.
        return (v1.x == v2.x && v1.y == v2.y);
    }
};

// Constructor with variables. No default constructor needed.
// It first extracts the UNIQUE coordinates and vertices from the lines vector, to properly pass them to CDT.
Navmesh::Navmesh(const std::vector<Line> lines) 
{
    // Map linking every point to an index, this helps isolate unique points.
    // It is also useful for determining which point connects to which in CDT.
    std::unordered_map<Vector2, size_t, Vector2Hash, Vector2Equals> pointToIndex;

    // Standard library vector2 for storing all unique points.
    
    // COMMENTED OUT FOR TESTING
    // std::vector<Vector2> uniquePoints;



    // Loop through every line made by Marching Squares, use const reference as a minor memory optimisation.
    for (const Line& line : lines) 
    {
        // Check if the first point in the line already exists within our map.
        if (pointToIndex.find(line.GetPosition()) == pointToIndex.end()) 
        {
            // If not, get the next available index of our new vector of unique points.
            size_t newIndex = uniquePoints.size();
            // Add the new index to our map to remember this point.
            pointToIndex[line.GetPosition()] = newIndex;
            // And add said point to our vector of unique points.
            uniquePoints.push_back(line.GetPosition());
        }
        
        // Same function for the last point of the line.
        if (pointToIndex.find(line.GetEndPos()) == pointToIndex.end()) 
        {
            size_t newIndex = uniquePoints.size();
            pointToIndex[line.GetEndPos()] = newIndex;
            uniquePoints.push_back(line.GetEndPos());
        }
    }




};

// Simple Drawing function.
void Navmesh::Draw() 
{
    // CURRENT VERSION ONLY FOR TESTING PURPOSES.
    for (const Vector2& point : uniquePoints) {DrawCircleV(point, 2, GREEN);}
};