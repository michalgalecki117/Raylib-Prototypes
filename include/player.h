#pragma once

#include <memory>
#include <cmath>

#include "rendering.h"
#include "shapes.h"
#include "transform.h"

class MovementCore // Movement engine of simple moving objects. 
{
private:
    // Reference to parent object's transform.
    ObjectTransform& transform;
    // Object's speed and direction.
    float speed;
    float direction;
    float targetDirection;

    // Private constructor.
    // Transform not constant as the movement engine should modify it.    
    MovementCore(ObjectTransform& transform);

public:
    // Public factories.
    static MovementCore makeMovementCore(ObjectTransform& transform);

    // Simple Getters and Setters.
    float getSpeed() const { return speed; }
    void setSpeed(float speed);

    float getDirection() const { return direction; }
    void setTargetDirection(float direction) { this->targetDirection = direction; }
    void setTargetDirectionDegrees(float direction);

    // Update movement.
    void update();
};

class Player // Main player class. 
{
private:
    // Stores it's own renderer and possibly texture.
    std::unique_ptr<Drawable> renderer;
    Texture2D texture;
    // Aswell as the shape it uses for collisions.
    std::unique_ptr<Shape> collisionShape;
    // Aswelll as it's own transform to be modified by said MovementCore.
    ObjectTransform transform;
    // Stores a logic component, MovementCore responsible for moving the object.
    MovementCore movement;

    // Class variable to check if player object already exists.
    static inline bool exists = false;

    // Private constructor.
    Player(std::unique_ptr<Drawable> renderer, std::unique_ptr<Shape> collisionShape, ObjectTransform transform);

public:
    // Public factories.
    static std::unique_ptr<Player> makePlayer(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, Color color = BLACK);
    static std::unique_ptr<Player> makePlayer(Texture2D texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform);

    // Public destructor redifined.
    ~Player() { exists = false; }

    // Delegation to movement component.
    float getSpeed() const { return movement.getSpeed(); }
    void setSpeed(float speed) { movement.setSpeed(speed); }
    float getDirection() const { return movement.getDirection(); }
    void setTargetDirection(float direction) { movement.setTargetDirection(direction); }
    void setTargetDirectionDegrees(float direction) {movement.setTargetDirectionDegrees(direction); }

    const Drawable& getRenderer() const { return *renderer; }
    const ObjectTransform& getTransform() const { return transform; }

    void update() { movement.update(); }
};