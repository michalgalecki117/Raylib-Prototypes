#include "obstacle.h"
#include <cmath>

// Constructor with variables.
Obstacle::Obstacle(
    std::unique_ptr<Drawable> renderer, 
    std::unique_ptr<Shape> collisionShape, 
    ObjectTransform transform) 
        : renderer(std::move(renderer)), 
        collisionShape(std::move(collisionShape)),
        transform(transform)
    {};

// Factory.
Obstacle Obstacle::makeObstacle(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, Color color)
{
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(color, *collisionShape);

    // Call the actual private constructor.
    return Obstacle(std::move(renderer),std::move(collisionShape), transform);
}
// Factory with a Texture2D texture.
Obstacle Obstacle::makeObstacle(const Texture2D& texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform)
{
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(texture);
    // Call the actual private constructor.
    return Obstacle(std::move(renderer), std::move(collisionShape), transform);
}

// rotateTo a given angle directly.
void Obstacle::rotateTo(float angle) 
{
    transform.setRotation(angle);
}

// rotateBy a certain angle, either clockwise or counterclockwise.
void Obstacle::rotateBy(float angle, bool clockwise) 
{
    // Get current rotation first.
    float current = transform.getRotation();
    if (clockwise) 
    {   
        // Then rotate by given angle.
        transform.setRotation(fmod(current + angle, 360.0f));
    }
    else 
    {
        transform.setRotation(fmod(current - angle,  360.0f));
    }
}

void Obstacle::moveTo(Vector2 position) 
{
    transform.setPosition(position);
}

void Obstacle::moveBy(float distance) 
{
    Vector2 currentPos = transform.getPosition();
    float currentRot = transform.getRotationRad();

    float deltaX = cos(currentRot) * distance;
    float deltaY = sin(currentRot) * distance;

    transform.setPosition({
        currentPos.x + deltaX,
        currentPos.y + deltaY});
}

// Simple refresh function to remake a renderer for a new texture.
void Obstacle::refreshRenderer(const Texture2D& texture) 
{
    renderer = Drawable::makeDrawer(texture);
}

// Simple drawing.
void Obstacle::draw() { renderer->draw(transform); }