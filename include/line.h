#pragma once
#include <raylib.h>
#include <vector>

#include "gameobject.h"

class Line : public GameObject // Simple Line gameobject.
{
public:
    Line(); // Default constructor.
    Line(Vector2 startPos, Vector2 endPos, Color startColor); // Constructor with variables.
    virtual ~Line() = default; // Default destructor.

    // Simple Getters and Setters.
    Vector2 GetEndPos() const;
    void SetEndPos(Vector2 endPos);

    // Simple update and draw functions.
    void update();
    void Draw() const override;
    
protected:
    // A line needs an endPos in addition to startPos.
    Vector2 endPos;
};