#include "line.h"
#include <raylib.h>
#include <cmath>

Line::Line() : GameObject(), endPos({0.0f, 0.0f}) {} // Default constructor.

Line::Line(Vector2 startPos, Vector2 startendPos, Color startColor) 
    : GameObject(startPos, startColor)
    , endPos(startendPos)
{} // Constructor with variables.

// Simple Getters and Setters.
Vector2 Line::GetEndPos() const { return endPos; } 
void Line::SetEndPos(Vector2 newEndPos) { endPos = newEndPos; }

void Line::update() 
{} // Potential update function for later use.

// Simple draw function.
void Line::Draw() const 
{
    DrawLineEx(position, endPos, 2.0f ,color);
}