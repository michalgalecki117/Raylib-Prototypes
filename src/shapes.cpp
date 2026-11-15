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
    return std::unique_ptr<Shape>(new TPolygon(vertices, origin));
}