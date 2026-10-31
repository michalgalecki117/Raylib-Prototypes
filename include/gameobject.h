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