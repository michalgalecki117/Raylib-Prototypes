#include "box.h"
#include <raylib.h>
#include <cmath>

Box::Box() : GameObject(), size({0,0}) {} // Default constructor.

Box::Box(Vector2 startPos, Vector2 startSize, Color startColor) 
    : GameObject(startPos, startColor)
    , size(startSize)
{} // Constructor with variables.

// Simple Getters and Setters.
Vector2 Box::GetSize() const { return size; }
void Box::SetSize(Vector2 newSize) { size = newSize; }

// Rectangle Getter.
Rectangle Box::GetRectangle() const { return {position.x, position.y, size.x, size.y}; }

void Box::update() 
{} // Potential update function for later use.

// Simple box drawing function.
void Box::Draw() const 
{
    DrawRectangleV(position, size, color);
}