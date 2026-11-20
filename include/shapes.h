#pragma once
#include "raylib.h"
#include <memory>
#include <vector>
#include "mesh.h"

class Shape // Shape Interface
{
public:
    // Purely virtual functions for shapes to inherit.

    // Set default destructor just in case.
    virtual ~Shape() = default;
    

    static std::unique_ptr<Shape> makeCircle(float radius);
    static std::unique_ptr<Shape> makeRectangle(Vector2 size);
    static std::unique_ptr<Shape> makeTriangle(Vector2 pos1, Vector2 pos2, Vector2 pos3);
    static std::unique_ptr<Shape> makeLine(Vector2 pos1, Vector2 pos2);
    static std::unique_ptr<Shape> makePolygon(std::vector<Vector2> vertices, Vector2 origin);
};

// Implementation of Shape interface, Circle class.
class TCircle : public Shape
{
private:
    // By itself, a circle shape only has it's radius.
    float radius;

    // Keeping the constructor private.
    // Only the interface factory method can create a new Circle.
    TCircle(float radius)
        : radius(radius) {}

    // Permit access to constructor only to Shape factory.
    friend std::unique_ptr<Shape> Shape::makeCircle(float radius);

public:
    // Simple Getters and Setters.
    float getRadius() const { return radius; }
    void setRadius(float newRadius) { if (newRadius > 0) { radius = newRadius; } }
};

// Implementation of Shape interface, Rectangle class.
class TRectangle : public Shape
{
private:
    // By itself, a rectangle shape only has it's size vector.
    Vector2 middle;
    Vector2 size;

    // Keeping the constructor private.
    // Only the interface factory method can create a new Rectangle.
    TRectangle(Vector2 size)
        : size(size)
    {
        middle = {size.x / 2.0f, size.y / 2.0f};
    }

    // Permit access to constructor only to Shape factory.
    friend std::unique_ptr<Shape> Shape::makeRectangle(Vector2 size);

public:
    // Simple Getters and Setters.
    Vector2 getMiddle() const { return middle; }
    Vector2 getSize() const { return size; }
    void setSize(Vector2 newSize)
    { 
        if (newSize.x > 0 && newSize.y > 0) 
        { 
            size = newSize;
            middle = {size.x / 2.0f, size.y / 2.0f};
        } 
    }
};

// Implementation of Shape interface, Triangle class.
class TTriangle : public Shape
{
private:
    // By itself, a triangle shape only has it's 3 vertices.
    // In addition, we save the position of the midpoint.
    Vector2 middle;
    Vector2 pos1;
    Vector2 pos2;
    Vector2 pos3;

    // Keeping the constructor private.
    // Only the interface factory method can create a new Triangle.
    TTriangle(Vector2 pos1, Vector2 pos2, Vector2 pos3)
        : pos1(pos1), pos2(pos2), pos3(pos3)
    {
        middle = {(pos1.x + pos2.x + pos3.x) / 3.0f, (pos1.y + pos2.y + pos3.y) / 3.0f};
    }

    // Permit access to constructor only to Shape factory.
    friend std::unique_ptr<Shape> Shape::makeTriangle(Vector2 pos1, Vector2 pos2, Vector2 pos3);

public:
    // Simple Getters and Setters.
    std::vector<Vector2> getVertices() const { return {pos1, pos2, pos3}; }
    Vector2 getMiddle() const { return middle; }
    void setVertices(std::vector<Vector2> newVertices)
    {
        pos1 = newVertices[0];
        pos2 = newVertices[1];
        pos3 = newVertices[2];
        middle = {(pos1.x + pos2.x + pos3.x) / 3.0f, (pos1.y + pos2.y + pos3.y) / 3.0f};
    }
};

// Implementation of Shape interface, Line class.
class TLine : public Shape
{
private:
    // By itself, a line shape only has it's 2 vertices.
    // In addition, we save the position of the midpoint.
    Vector2 middle;
    Vector2 pos1;
    Vector2 pos2;

    // Keeping the constructor private.
    // Only the interface factory method can create a new Line.
    TLine(Vector2 pos1, Vector2 pos2)
        : pos1(pos1), pos2(pos2)
    {
        middle = {(pos1.x + pos2.x) / 2.0f, (pos1.y + pos2.y) / 2.0f};
    }

    // Permit access to constructor only to Shape factory.
    friend std::unique_ptr<Shape> Shape::makeLine(Vector2 pos1, Vector2 pos2);

public:
    // Simple Getters and Setters.
    std::vector<Vector2> getVertices() const { return {pos1, pos2}; }
    Vector2 getMiddle() const { return middle; }
    void setVertices(std::vector<Vector2> newVertices)
    {
        pos1 = newVertices[0];
        pos2 = newVertices[1];
        middle = {(pos1.x + pos2.x) / 2.0f, (pos1.y + pos2.y) / 2.0f};
    }
};

// Implementation of Shape interface, Polygon class.
class TPolygon : public Shape
{
private:
    // By itself, a polygon shape only has it's n vertices.
    // In addition, we save the position of the midpoint.
    Vector2 origin;
    mesh::Mesh mesh;

    // Keeping the constructor private.
    // Only the interface factory method can create a new Polygon.
    TPolygon(mesh::Mesh mesh, Vector2 origin)
        : origin(origin), mesh(mesh)
    {}

    // Permit access to constructor only to Shape factory.
    friend std::unique_ptr<Shape> Shape::makePolygon(std::vector<Vector2> vertices, Vector2 origin);

public:
    // Simple Getters and Setters.
    mesh::Mesh getMesh() const { return mesh; }
    Vector2 getOrigin() const { return origin; }
};