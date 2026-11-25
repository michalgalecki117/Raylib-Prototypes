#include "obstacle.h"
#include <cmath>

// Constructor with variables.
GameObject::GameObject(
    std::unique_ptr<Drawable> renderer, 
    std::unique_ptr<Shape> collisionShape, 
    ObjectTransform transform,
    bool collider, bool overrideTerrain) 
        : renderer(std::move(renderer)), 
        collisionShape(std::move(collisionShape)),
        transform(transform),
        collider(collider),
        overrideTerrain(overrideTerrain)
    {};

// Factory.
GameObject GameObject::makeGameObject(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, Color color)
{
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(color, *collisionShape);

    // Call the actual private constructor.
    return GameObject(std::move(renderer),std::move(collisionShape), transform);
}
// Factory with a Texture2D texture.
GameObject GameObject::makeGameObject(const Texture2D& texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform)
{
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(texture);
    // Call the actual private constructor.
    return GameObject(std::move(renderer), std::move(collisionShape), transform);
}

GameObject GameObject::makeGameObject(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, bool collider, Color color)
{
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(color, *collisionShape);

    // Call the actual private constructor.
    return GameObject(std::move(renderer),std::move(collisionShape), transform, collider);
}
// Factory with a Texture2D texture.
GameObject GameObject::makeGameObject(const Texture2D& texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform, bool collider)
{
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(texture);
    // Call the actual private constructor.
    return GameObject(std::move(renderer), std::move(collisionShape), transform, collider);
}

GameObject GameObject::makeGameObject(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, bool collider, bool overrideTerrain, Color color)
{
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(color, *collisionShape);

    // Call the actual private constructor.
    return GameObject(std::move(renderer),std::move(collisionShape), transform, collider, overrideTerrain);
}
// Factory with a Texture2D texture.
GameObject GameObject::makeGameObject(const Texture2D& texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform, bool collider, bool overrideTerrain)
{
    // Creating a renderer using the Drawable interface factory.
    std::unique_ptr<Drawable> renderer =  Drawable::makeDrawer(texture);
    // Call the actual private constructor.
    return GameObject(std::move(renderer), std::move(collisionShape), transform, collider, overrideTerrain);
}

// rotateTo a given angle directly.
void GameObject::rotateTo(float angle) 
{
    transform.setRotation(angle);
}

// rotateBy a certain angle, either clockwise or counterclockwise.
void GameObject::rotateBy(float angle, bool clockwise) 
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

void GameObject::moveTo(Vector2 position) 
{
    transform.setPosition(position);
}

void GameObject::moveBy(float distance) 
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
void GameObject::refreshRenderer(const Texture2D& texture) 
{
    renderer = Drawable::makeDrawer(texture);
}

// Simple drawing.
void GameObject::draw() { renderer->draw(transform); }