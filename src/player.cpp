#include "player.h"

#include <vector>
#include <raylib.h>
#include <cmath>

Player::Player(Vector2 startPos, float startRad, Color startColor, float startSpeed, float startDirection) 
    : GameObject(startPos, startColor)
    , speed(startSpeed)
    , direction(startDirection)
    , radians(startDirection * (PI / 180.0f))
    , radius(startRad)
{} // Constructor with variables. Calculates radians automatically.

// Simple circle draw function as player object is a circle.
void Player::Draw() const 
{
    DrawCircleV(position, radius, color);
}

// Player update function.
void Player::update(const std::vector<Box>& boxes)
/*
Updates player position and speed every frame regardless of input.
Further cancels out player movement in case of collision with different objects.
*/
{
    // Decrease the speed every frame with no input.
    speed *= 0.9f;

    // Window dimensions variables for border collision detection.
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    // Calculate movement change based on direction and speed.
    float deltaX = std::cos(radians) * speed;
    float deltaY = std::sin(radians) * speed;

    // 1. MOVE & CHECK X-AXIS
    // ----------------------
    position.x += deltaX;
    
    // Screen X Bounds
    // Right.
    if (position.x + radius >= screenWidth) {
        // Reduce speed to 0 and move player away from border.
        position.x = screenWidth - radius;
        speed = 0.0f;
    } 
    // Left.
    else if (position.x - radius <= 0) {
        // Reduce speed to 0 and move player away from border.
        position.x = radius;
        speed = 0.0f;
    }
    
    // Box X Collisions (Revert X if we hit a box)
    for (const auto& box : boxes) 
    {
        if (CheckCollisionCircleRec(position, radius, box.GetRectangle()))
        {
            position.x -= deltaX; // Undo the X movement to stop before impact
            speed = 0.0f;
            break;
        }
    }

    // 2. MOVE & CHECK Y-AXIS
    // ----------------------
    position.y += deltaY;
    
    // Screen Y Bounds
    // Top.
    if (position.y + radius >= screenHeight) {
        // Reduce speed to 0 and move player away from border.
        position.y = screenHeight - radius;
        speed = 0.0f;
    }
    // Bottom.
    else if (position.y - radius <= 0) {
        // Reduce speed to 0 and move player away from border.
        position.y = radius;
        speed = 0.0f;
    }
    
    // Box Y Collisions (Revert Y if we hit a box)
    for (const auto& box : boxes) 
    {
        if (CheckCollisionCircleRec(position, radius, box.GetRectangle()))
        {
            position.y -= deltaY; // Undo the Y movement to stop before impact
            speed = 0.0f;
            break;
        }
    }
}

// Player update function with key input.
void Player::UpdateWithKeys(int target_direction)
/*
Simply updates the players direction and speed to reflect user input.
Smooths out rotation to the target_direction to avoid jittery movement.
*/
{
    // Ignore smoothing out under very low speeds.
    if (speed <= 2.0f) 
    {
        // Directly set the new target_direction as the current direction.
        radians = target_direction * (PI / 180.0f);
        direction = target_direction;
    }
    // Smooth out rotation.
    else if (speed >= 2.0f) 
    {
        // Calculate the difference in direction, between current and target.
        float difference = target_direction - direction;
        // Ensure difference rotates the shortest way. Not always to the right or left.
        difference = std::fmod(difference + 540.0f, 360.0f) - 180.0f;
        // Smooth out the transition over time by only rotating a fraction of the difference.
        direction += (difference * 0.20f);
        // Ensure direction in degrees.
        direction = std::fmod(direction, 360.0f);
        // Ensure positive direction.
        if (direction < 0.0f) { direction += 360.0f; }
        // Convert back to radians for simple movement math.
        radians = direction * (PI / 180.0f);
    }

    // Set new speed after input.
    speed = 5.0f;
}




