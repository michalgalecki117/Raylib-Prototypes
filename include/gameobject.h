#pragma once
#include <raylib.h>

class GameObject // Base class of every game object in the game world.
{
public:

    GameObject();  // Default constructor.
    GameObject(Vector2 startPos, Color color); // Full variable constructor.
    virtual ~GameObject() = default; // Default destructor.
    
    // Getters and Setters.
    Vector2 GetPosition() const;
    void SetPosition(Vector2 newPos);
    
    float GetRadius() const;
    void SetRadius(float newRad);
    
    Color GetColor() const;
    void SetColor(Color newColor);

    // Purely virtual draw function to be inherited.
    virtual void Draw() const;
    
protected:
    // Every object has to have a position to exist within the game world.
    // Every other trait is to be decided by inheritor classes.
    Vector2 position;
    // While useless to a base gameobject, 
    // a base color is used by every single type of gameobject inheritor class.
    Color color;
};