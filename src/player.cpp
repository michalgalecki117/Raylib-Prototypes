#include "player.h"

//-------------------------------------------------------------------------------------------------------------------------------
// MovementCore Class. 
//-------------------------------------------------------------------------------------------------------------------------------

// Private constructor.
MovementCore::MovementCore(ObjectTransform& transform)
    : transform(transform), speed(0.0f), direction(0.0f)
{}

// Public factories.
MovementCore MovementCore::makeMovementCore(ObjectTransform& transform) 
{
    return MovementCore(transform);
}

void MovementCore::setSpeed(float speed) 
{
    if (speed >= 0.0f) {this->speed = speed;}
}

void MovementCore::setTargetDirectionDegrees(float direction) 
{
    // Ensure degrees positive and modulo 360.
    float degrees = fmod(direction, 360.0f);
    if (degrees < 0.0f) degrees += 360.0f;

    this->targetDirection = degrees * (PI / 180.0f);
}

/*
Moveable object update function.
Updates object position and speed every frame regardless of input.
Further cancels out object movement in case of collision with different objects.
*/
void MovementCore::update() 
{
    // Get current transform properties of object.
    Vector2 position = transform.getPosition();
    float rotation = transform.getRotationRad();

    // Decrease the speed every frame with no input.
    speed *= 0.9f;

    // Ignore smoothing out under very low speeds.
    if (speed <= 0.5f) 
    {
        // Directly set the new target_direction as the current direction.
        direction = targetDirection;
    }
    // Smooth out rotation.
    else if (speed >= 1.0f)
    {
        // Calculate the difference in direction, between current and target.
        float difference = fmod(targetDirection - direction + 3.0f * PI, 2.0f * PI);
        // Ensure difference rotates the shortest way. Not always to the right or left.
        if (difference < 0.0f) { difference += 2.0f * PI; }
        difference -= PI;
        // Smooth out the transition over time by only rotating a fraction of the difference.
        direction += (difference * 0.20f);
    }

    // Calculate movement change based on direction and speed.
    float deltaX = std::cos(direction) * speed;
    float deltaY = std::sin(direction) * speed;

    position.x += deltaX;
    position.y += deltaY;

    transform.setPosition(position);
    transform.setRotationRad(rotation);
}

//-------------------------------------------------------------------------------------------------------------------------------
// Player Class. 
//-------------------------------------------------------------------------------------------------------------------------------

// Constructor with variables.
Player::Player(
    std::unique_ptr<Drawable> renderer, 
    std::unique_ptr<Shape> collisionShape, 
    ObjectTransform transform) 
        : renderer(std::move(renderer)), 
        collisionShape(std::move(collisionShape)),
        transform(transform),
        movement(MovementCore::makeMovementCore(this->transform))
    {};

// Factory.
std::unique_ptr<Player> Player::makePlayer(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, Color color)
{
    if (exists) { return nullptr; }
    exists = true;
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(color, *collisionShape);

    // Call the actual private constructor.
    return std::unique_ptr<Player>(new Player(std::move(renderer),std::move(collisionShape), transform));
}
// Factory with a Texture2D texture.
std::unique_ptr<Player> Player::makePlayer(Texture2D texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform)
{
    if (exists) { return nullptr;}
    exists = true;
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(texture);
    // Call the actual private constructor.
    return std::unique_ptr<Player>(new Player(std::move(renderer), std::move(collisionShape), transform));
}

