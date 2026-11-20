#pragma once
#include "raylib.h"

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
    ObjectTransform(Vector2 position, float rotation = 0.0f, float scale = 1.0f);
    // Simple Getters and Setters.
    Vector2 getPosition() const { return position; }
    void setPosition(Vector2 newPosition) { position = newPosition; }

    float getRotation() const { return rotation; }
    void setRotation(float newRotation) { rotation = newRotation; }
    // Radian versions.
    float getRotationRad() const;
    void setRotationRad(float newRotationRad);

    float getScale() const { return scale; }
    void setScale(float newScale);
};
