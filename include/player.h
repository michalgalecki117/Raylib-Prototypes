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
    Player(Vector2 startPos, float startRad, Color startColor, float startSpeed, float startDirection); 
    
    // Update function regardless of input. (Affects position, speed and direction).
    void update(const std::vector<Box>& boxes);

    // Update function based on input. (Affects position, speed and direction).
    void UpdateWithKeys(int target_direction);

private:
    // Player object being capable of movement, it requires a speed and direction.
    float speed;
    float direction;
    // Direction in degrees converted to radians to simplify math operations.
    float radians;
};
