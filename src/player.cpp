#include "player.h"

#include <vector>
#include <raylib.h>
#include <cmath>

Player::Player(Vector2 startPos, float startRad, Color startColor, float startSpeed, float startDirection) 
    : Circle(startPos, startRad, startColor)
    , speed(startSpeed)
    , direction(startDirection)
{} // Constructor with variables. Calculates radians automatically.

// Simple Getters and Setters.
float Player::GetDirectionDegrees() const 
{
    float degrees = direction * (180.0f / PI);
    return fmod(degrees, 360.0f);
}
void Player::SetDirectionDegrees(float newDegrees) 
{
    // Ensure degrees positive and modulo 360.
    float degrees = fmod(newDegrees, 360.0f);
    if (degrees < 0.0f) degrees += 360.0f;

    direction = degrees * (PI / 180.0f);
}

// Player update function.
void Player::update(const std::vector<Box>& boxes, const std::vector<Line>& vertices)
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
    float deltaX = std::cos(direction) * speed;
    float deltaY = std::sin(direction) * speed;

    // 1. MOVE & CHECK X-AXIS
    // ----------------------
    position.x += deltaX;
    
    // Screen X Bounds
    // Right.
    if (position.x + GetRadius() >= screenWidth) {
        // Reduce speed to 0 and move player away from border.
        position.x = screenWidth - GetRadius();
        speed = 0.0f;
    } 
    // Left.
    else if (position.x - GetRadius() <= 0) {
        // Reduce speed to 0 and move player away from border.
        position.x = GetRadius();
        speed = 0.0f;
    }
    
    // Box X Collisions (Revert X if we hit a box)
    for (const auto& box : boxes) 
    {
        if (CheckCollisionCircleRec(position, GetRadius(), box.GetRectangle()))
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
    if (position.y + GetRadius() >= screenHeight) {
        // Reduce speed to 0 and move player away from border.
        position.y = screenHeight - GetRadius();
        speed = 0.0f;
    }
    // Bottom.
    else if (position.y - GetRadius() <= 0) {
        // Reduce speed to 0 and move player away from border.
        position.y = GetRadius();
        speed = 0.0f;
    }
    
    // Box Y Collisions (Revert Y if we hit a box)
    for (const auto& box : boxes) 
    {
        if (CheckCollisionCircleRec(position, GetRadius(), box.GetRectangle()))
        {
            position.y -= deltaY; // Undo the Y movement to stop before impact
            speed = 0.0f;
            break;
        }
    }


    // Border collisions along all axis. (Revert both x and y on border hit)
    for (const auto& line : vertices) 
    {
        if (CheckCollisionCircleLine(position, GetRadius(), line.GetPosition(), line.GetEndPos()))
        {
            position.x -= deltaX;
            position.y -= deltaY;
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
    if (speed <= 1.0f) 
    {
        // Directly set the new target_direction as the current direction.
        SetDirectionDegrees(target_direction);
    }
    // Smooth out rotation.
    else if (speed >= 1.0f) 
    {
        // Get current direction in degrees.
        float degrees = GetDirectionDegrees();
        // Calculate the difference in direction, between current and target.
        float difference = target_direction - degrees;
        // Ensure difference rotates the shortest way. Not always to the right or left.
        difference = fmod(difference + 540.0f, 360.0f) - 180.0f;
        // Smooth out the transition over time by only rotating a fraction of the difference.
        degrees += (difference * 0.20f);
        // Convert back to radians for simple movement math.
        SetDirectionDegrees(degrees);
    }

    // Set new speed after input.
    speed = 2.0f;
}




