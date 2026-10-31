#pragma once
#include <raylib.h>

class GameObject // Base class of every game object in the game world.
{
public:

    GameObject() : position{0,0}, color(WHITE) {}  // Default constructor.
    GameObject(Vector2 startPos, Color startColor)     
    : position(startPos)
    , color(startColor)
    {} // Constructor with variables.
    virtual ~GameObject() = default; // Default destructor.
    
    // Simple Getters and Setters.
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 newPos) { position = newPos; }

    Color GetColor() const { return color; }
    void SetColor(Color newColor) { color = newColor; }

    // Purely virtual draw function to be inherited.
    virtual void Draw() const = 0;
    
protected:
    // Every object has to have a position to exist within the game world.
    // Every other trait is to be decided by inheritor classes.
    Vector2 position;
    // While useless to a base gameobject, 
    // a base color is used by every single type of gameobject inheritor class.
    Color color;
};

class Line : public GameObject // Simple Line gameobject.
{
public:
    Line() : GameObject(), endPos({0.0f, 0.0f}) {} // Default constructor.
    Line(Vector2 startPos, Vector2 startendPos, Color startColor) 
    : GameObject(startPos, startColor)
    , endPos(startendPos)
    {} // Constructor with variables.
    virtual ~Line() = default; // Default destructor.

    // Simple Getters and Setters.
    Vector2 GetEndPos() const { return endPos; } 
    void SetEndPos(Vector2 newEndPos) { endPos = newEndPos; }

    // Simple draw function.
    void Draw() const override { DrawLineEx(position, endPos, 2.0f ,color); }
    
protected:
    // A line needs an endPos in addition to startPos.
    Vector2 endPos;
};

class Box : public GameObject // Simple rectangle gameobject.
{
public:
    Box() : GameObject(), size({0,0}) {} // Default constructor.
    Box(Vector2 startPos, Vector2 startSize, Color startColor) 
    : GameObject(startPos, startColor)
    , size(startSize)
    {} // Contructor with variables.
    virtual ~Box() = default; // Default destructor.

    // Simple Getters and Setters.
    Vector2 GetSize() const { return size; }
    void SetSize(Vector2 newSize) { size = newSize; }

    // Getter for the custom raylib object, Rectangle.
    Rectangle GetRectangle() const { return {position.x, position.y, size.x, size.y}; }
    // Simple box drawing function.
    void Draw() const { DrawRectangleV(position, size, color); }
    
protected:
    // Box contains both starting position and dimensions only.
    Vector2 size;
};

class Circle : public GameObject // Simple circle gameobject.
{
public:
    Circle() : GameObject(), radius(0) {} // Default constructor.
    Circle(Vector2 startPos, int startRad, Color startColor) 
    : GameObject(startPos, startColor)
    , radius(startRad)
    {} // Contructor with variables.
    virtual ~Circle() = default; // Default destructor.

    // Simple Getters and Setters.
    int GetRadius() const { return radius; }
    void SetRadius(int newRad) { radius = newRad; }

    // Simple box drawing function.
    void Draw() const { DrawCircleV(position, radius, color); }
    
protected:
    // Box contains both starting position and dimensions only.
    int radius;
};