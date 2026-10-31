#pragma once
#include <raylib.h>
#include "gameobject.h"

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