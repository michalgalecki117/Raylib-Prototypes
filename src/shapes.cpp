#include <memory>
#include "shapes.h"

std::unique_ptr<Shape> Shape::makeCircle(float radius) 
{
    return std::unique_ptr<Shape>(new TCircle(radius));
}

std::unique_ptr<Shape> Shape::makeRectangle(Vector2 size) 
{
    return std::unique_ptr<Shape>(new TRectangle(size));
}

std::unique_ptr<Shape> Shape::makeTriangle(Vector2 pos1, Vector2 pos2, Vector2 pos3) 
{
    return std::unique_ptr<Shape>(new TTriangle(pos1, pos2, pos3));
}

std::unique_ptr<Shape> Shape::makeLine(Vector2 pos1, Vector2 pos2) 
{
    return std::unique_ptr<Shape>(new TLine(pos1, pos2));
}

std::unique_ptr<Shape> Shape::makePolygon(std::vector<Vector2> vertices, Vector2 origin) 
{
    // Vector to hold points of cdt format.
    std::vector<CDT::V2d<double>> cdtPoints;

    // Vector to hold edges of cdt format.
    std::vector<CDT::Edge> edges;

    // Convert all Vector2 vertices into CDT vectors
    for (const Vector2& point : vertices) 
    {   
        cdtPoints.push_back({
        static_cast<double>(point.x)
        ,static_cast<double>(point.y)
        });
    }

    // Construct shape contours by connecting each point to the next as edges.
    size_t size = cdtPoints.size();
    for (size_t i = 0; i < size; i++) 
    {
        CDT::VertInd a = static_cast<CDT::VertInd>(i);
        CDT::VertInd b = static_cast<CDT::VertInd>((i + 1) % size);
        edges.push_back({a, b}); 
    }

    // Initialising CDT triangulation.
    CDT::Triangulation<double> cdt;
    cdt.insertVertices(cdtPoints);
    cdt.insertEdges(edges);
    cdt.eraseOuterTrianglesAndHoles();

    // Fill mesh with unconstrained delaunay triangulation.
    // I.E triangles to connect all edges.
    std::vector<CDT::V2d<double>> points = cdt.vertices;
    std::vector<CDT::Triangle> triangles = cdt.triangles;
    
    return std::unique_ptr<Shape>(new TPolygon(points, triangles, origin));
}