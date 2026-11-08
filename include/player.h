#pragma once
#include <vector>
#include <raylib.h>
#include "geometry.h"

class Player : public Circle // Player gameobject. 
/*
Currently the only object class capable of movement 
and limited interaction with different objects.
*/
{
public:
    // Constructor with variables.
    // There is no default constructor as player position will depend on created world object.
    Player(Vector2 startPos, float startRad, Color startColor, float startSpeed, float startDirectionDegrees); 
    
    // Simple Getters and Setters.
    float GetDirectionDegrees() const;
    void SetDirectionDegrees(float newDegrees);

    float GetDirectionRadians() const { return direction; }
    void SetDirectionRadians(float newRadians) { direction = newRadians; }
    
    // Update function regardless of input. (Affects position, speed and direction).
    void update(const std::vector<Box>& boxes, const std::vector<Line>& vertices);

    // Update function based on input. (Affects position, speed and direction).
    void UpdateWithKeys(int target_direction);

private:
    // Player object being capable of movement, it requires a speed and direction.
    float speed;
    // Direction directly in radians to simplify math operations.
    float direction;
};
