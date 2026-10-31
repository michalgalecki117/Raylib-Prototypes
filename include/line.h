#pragma once
#include <raylib.h>
#include <vector>

#include "gameobject.h"

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