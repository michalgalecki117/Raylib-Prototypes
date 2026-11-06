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

// Private function for interpolating points based on a given threshold.
// Basically it finds more accurate points to connect the vertices to rather than just running them through the corners themselves.
// This allows our lines to have different angles.
// Also snaps points to the border if intersecting.
Vector2 World::Interp(Vector2 p1, Vector2 p2, double v1, double v2, Vector2 dimensions) const 
{
    // Choose one of the points if both are superimposed over each other.
    if (std::abs(v1 - v2) < 0.00001f) return p1;

    // Calculate the fractional distance between the two points and the threshold.
    double t = (threshold - v1) / (v2 - v1);

    // Create the new point created along the p1, p2 edge based on the fractional distance using threshold.
    float rawX  = static_cast<float>(p1.x + t * (p2.x - p1.x));
    float rawY = static_cast<float>(p1.y + t * (p2.y - p1.y));
    
    // Snap points to the borders if edge intersects it.
    if (rawX <= cellSize) { rawX = cellSize; }
    if (rawY <= cellSize) { rawY = cellSize; }
    if (rawX >= dimensions.x - cellSize) { rawX = dimensions.x - cellSize; }
    if (rawY >= dimensions.y - cellSize) { rawY = dimensions.y - cellSize; }

    // Round said point to the nearest pixel to avoid errors in later use, then return.
    float roundedX = round(rawX);
    float roundedY = round(rawY); 
    return {roundedX, roundedY};
};


// This function is used to build a segmented line across an axis.
// It starts in cornerA and ends in cornerB, making sure to create a line across every pair of points in pts vector.
// To do so, it must first sort all the points along either the X or Y axis.
void World::BuildBorderSegments(std::vector<Vector2>& pts, Vector2 cornerA, Vector2 cornerB, bool sortX) 
{
    // Manually add in both selected corners to the points pts vector.
    pts.push_back(cornerA);
    pts.push_back(cornerB);

    // Sort points along the chosen axis. (X or Y).
    sort(pts.begin(), pts.end(), [sortX](const Vector2& a, const Vector2& b) 
    {
        return sortX ? (a.x < b.x) : (a.y < b.y);
    });

    // Remove duplicate points (If multiple points somehow connect in the same place at the border.)
    pts.erase(unique(pts.begin(), pts.end(), [](const Vector2& a, const Vector2& b) 
        // If points too close together, erase one.
        { return abs(a.x - b.x) < 0.1f && abs(a.y - b.y) < 0.1f; })
        , pts.end()
    );

    // Create consecutive edge segments between every pair of points. 
    for (size_t i = 0; i < pts.size() - 1; i++) {
        lines.push_back(Line(pts[i], pts[i + 1], PURPLE));
    }
};

// Marching Squares algorithm generating vertices based on threshhold and perlin noise.
void World::GenerateVertices(Vector2 dimensions)
/*
Marching squares divides the entire space into a grid of cells the size of CellSize.
Then each corner is sampled from perlin noise and checked against the specified threshold,
With the whole cell checked, the line can be decided depending on correct corners.
This loops through the whole noise of given dimensions to produce an array of all interconnecting edges.
As long  as  it works, keep it ;)
*/
{
    // Destroys the lines vector before generating a new one.
    lines.clear();
    
    // Seperating given dimensions to simplify further math.
    int width = static_cast<int>(dimensions.x);
    int height = static_cast<int>(dimensions.y);

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
    for (int y = 0; y < height; y += cellSize) {
        for (int x = 0; x < width; x += cellSize) {
            
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
            Vector2 eTop = Interp(tl, tr, vTL, vTR, dimensions);
            Vector2 eRight = Interp(tr, br, vTR, vBR, dimensions);
            Vector2 eBottom = Interp(bl, br, vBL, vBR, dimensions);
            Vector2 eLeft = Interp(tl, bl, vTL, vBL, dimensions);

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
                lines.push_back(Line(pair.first, pair.second, RED));
                // Note the point coordinates if it was snapped to the border.
                // This is done to create border edges to enclose outer holes.
                if (abs(pair.first.y - cellSize) < 0.1f)                        { TopIntersects.push_back(pair.first); }
                if (abs(pair.first.x - cellSize) < 0.1f)                        { LeftIntersects.push_back(pair.first); }
                if (abs(pair.first.y - (dimensions.y - cellSize)) < 0.1f)       { BottomIntersects.push_back(pair.first); }
                if (abs(pair.first.x - (dimensions.x - cellSize)) < 0.1f)       { RightIntersects.push_back(pair.first); }

                if (abs(pair.second.y - cellSize) < 0.1f)                        { TopIntersects.push_back(pair.second); }
                if (abs(pair.second.x - cellSize) < 0.1f)                        { LeftIntersects.push_back(pair.second); }
                if (abs(pair.second.y - (dimensions.y - cellSize)) < 0.1f)       { BottomIntersects.push_back(pair.second); }
                if (abs(pair.second.x - (dimensions.x - cellSize)) < 0.1f)       { RightIntersects.push_back(pair.second); }
            }
        }
    }

    float minP = static_cast<float>(cellSize);
    float maxX = static_cast<float>(dimensions.x - cellSize);
    float maxY = static_cast<float>(dimensions.y - cellSize);

    // Define the four corners
    Vector2 topLeft     = { minP, minP };
    Vector2 topRight    = { maxX, minP };
    Vector2 bottomLeft  = { minP, maxY };
    Vector2 bottomRight = { maxX, maxY };

    // Build segmented lines for each of the 4 borders using our member function
    BuildBorderSegments(TopIntersects, topLeft, topRight, true);       // Top: sorted by X
    BuildBorderSegments(RightIntersects, topRight, bottomRight, false);  // Right: sorted by Y
    BuildBorderSegments(BottomIntersects, bottomLeft, bottomRight, true); // Bottom: sorted by X
    BuildBorderSegments(LeftIntersects, topLeft, bottomLeft, false);     // Left: sorted by Y
};

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
    
    // Vector to store all line end points.
    // CDT type points. Only indexes in the map and not actual coordinates.
    std::vector<CDT::V2d<double>> cdtPoints;

    // Vector to store all edges (lines).
    // CDT type edges. Only indexes in the map and not actual coordinates.
    std::vector<CDT::Edge> cdtEdges;

    // Initialising CDT triangulation.
    CDT::Triangulation<double> cdt(
        CDT::VertexInsertionOrder::Auto,
        CDT::IntersectingConstraintEdges::TryResolve,
        0.0
    );

    // Loop through every line made by Marching Squares, use const reference as a minor memory optimisation.
    for (const Line& line : lines) 
    {
        // Get actual coordinates of edge endings.
        Vector2 startPos = line.GetPosition();
        Vector2 endPos = line.GetEndPos();

        // Variable to store both points' new indexes in the vector.
        size_t i0 = 0;
        size_t i1 = 0;

        // Check if the first point in the line already exists within our map.
        if (pointToIndex.find(startPos) == pointToIndex.end()) 
        {
            // If not, get the next available index of our new vector of unique points.
            i0 = uniquePoints.size();
            // Add the new index to our map to remember this point.
            pointToIndex[startPos] = i0;
            // And add said point to our vector of unique points.
            // This would normally be directly made into CDT points for navmesh calculation, I used Vector2 for visualisation.
            // It will get converted later.
            uniquePoints.push_back(startPos);
        }
        else { i0 = pointToIndex[startPos]; } // Get index directly from map if already exists.
        
        // Same function for the last point of the line.
        if (pointToIndex.find(endPos) == pointToIndex.end()) 
        {
            i1 = uniquePoints.size();
            pointToIndex[endPos] = i1;
            uniquePoints.push_back(endPos);
        }
        else { i1 = pointToIndex[endPos]; }

        if (i0 == i1) {continue;} // Eliminate zero length lines.

        cdtEdges.push_back(CDT::Edge(
            static_cast<size_t>(i0),
            static_cast<size_t>(i1)
        ));

    }

    // Here, we convert fromm Vector2 points, to cdt points.
    for (const Vector2& point : uniquePoints) 
    {
        // Casting each member of Vector2 to double to fit in V2d of CDT.
        cdtPoints.push_back(CDT::V2d<double>
        {
            static_cast<double>(point.x),
            static_cast<double>(point.y)
        });
    }

    // Inserting our found points to find where triangle edges can possibly connect.
    cdt.insertVertices(cdtPoints);
    // Inserting our found edges to block triangles from passing through walls.
    cdt.insertEdges(cdtEdges);
    // Erasing Super Triangle above everything to get a proper mesh.
    // Also erasing Outer Triangles (out of bounds).
    // Also erasing holes in the map where the player cannot pass. (Between closed edge loops)
    cdt.eraseOuterTrianglesAndHoles();
    // Initialising class variables from CDT generated results.
    triangles = cdt.triangles;
    points = cdt.vertices;
};

// Simple Drawing function.
void Navmesh::DrawPoints() const 
{
    // Simply draws every unique point with a radius of 2 and in blue by default.
    for (const Vector2& point : uniquePoints) {DrawCircleV(point, 2, BLUE);}
};

// Navmesh Drawing function.
void Navmesh::DrawNavmesh() const
{
    // Gets every CDT triangle in navmesh.
    for (const CDT::Triangle& t : triangles) 
    {
        // Get all 3 indexes of the triangle points.
        size_t i0 = t.vertices[0];
        size_t i1 = t.vertices[1];
        size_t i2 = t.vertices[2];
    
        // Get the actual V2d point from points based on index.
        const CDT::V2d<double> pt0 = points[i0];
        const CDT::V2d<double> pt1 = points[i1];
        const CDT::V2d<double> pt2 = points[i2];

        // Convert V2d point to Vector2 point to allow drawing.
        Vector2 v0 = { static_cast<float>(pt0.x), static_cast<float>(pt0.y) };
        Vector2 v1 = { static_cast<float>(pt1.x), static_cast<float>(pt1.y) };
        Vector2 v2 = { static_cast<float>(pt2.x), static_cast<float>(pt2.y) };

        // Draw actual triangle overlay.
        DrawTriangle(v0, v2, v1, {50,100,50,50});
        // Draw triangle edge lines to differentiate between triangles.
        DrawTriangleLines(v0, v2, v1, {100,200,100,100});
    }
};