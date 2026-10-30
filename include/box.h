#pragma once
#include <raylib.h>
#include "gameobject.h"

class Box : public GameObject // Simple rectangle gameobject.
{
public:
    Box(); // Default constructor.
    Box(Vector2 startPos, Vector2 startSize, Color startColor); // Contructor with variables.
    virtual ~Box() = default; // Default destructor.

    // Simple Getters and Setters.
    Vector2 GetSize() const;
    void SetSize(Vector2 newSize);

    // Getter for the custom raylib object, Rectangle.
    Rectangle GetRectangle() const;

    // Simple update and draw functions.
    void update();
    void Draw() const override;
    
protected:
    // Box contains both starting position and dimensions only.
    Vector2 size;
};