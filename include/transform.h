#pragma once
#include <raylib.h>
#include <cmath>

class ObjectTransform // Transform component of all game objects.
{
private:
    // Simply holds and handles the modification of any objects
    // position, rotation or scale.
    Vector2 position;
    float rotation;
    float scale;
public:
    // Simple constructor with variables for the transform class.
    ObjectTransform(Vector2 position, float rotation = 0.0f , float scale = 1.0f)
        : position(position), rotation(rotation), scale(scale) {}

    // Simple Getters and Setters.
    Vector2 getPosition() const { return position; }
    void setPosition(Vector2 newPosition) { position = newPosition; }

    float getRotation() const { return rotation; }
    void setRotation(float newRotation) { rotation = newRotation; }

    float getScale() const { return scale; }
    void setScale(float newScale) 
    {  
        if (newScale > 0.0f && newScale < 20.0f) { scale = newScale; }
    }
};