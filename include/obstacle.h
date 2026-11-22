#pragma once
#include "raylib.h"
#include "rendering.h"
#include "shapes.h"
#include "transform.h"

#include <memory>

class Obstacle // Obstacle class to hold and manage world obstacles.
{
private:
    // Stores renderer for drawing itself.
    std::unique_ptr<Drawable> renderer;
    // Stores its shape, for collisions and drawing.
    std::unique_ptr<Shape> collisionShape;
    // Stores its position, rotation and scale.
    ObjectTransform transform;
    // Logic flag for collision checks.
    bool collider;
    // Logic flag for world edges creation.
    bool overrideTerrain;

    // Constructor with variables kept private for safe obstacle building.
    Obstacle(std::unique_ptr<Drawable> renderer, std::unique_ptr<Shape> collisionShape, ObjectTransform transform, bool collider = true, bool overrideTerrain = false);

public:
    // Public Obstacle factories for safe building.
    static Obstacle makeObstacle(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, Color color = BLACK);
    static Obstacle makeObstacle(const Texture2D& texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform);
    static Obstacle makeObstacle(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, bool collider, Color color = BLACK);
    static Obstacle makeObstacle(const Texture2D& texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform,  bool collider);
    static Obstacle makeObstacle(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, bool collider, bool overrideTerrain, Color color = BLACK);
    static Obstacle makeObstacle(const Texture2D& texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform,  bool collider, bool overrideTerrain);

    // Simple Getters and Setters.
    ObjectTransform getTransform() const { return transform; }
    Shape& getShape() const { return *collisionShape; }

    // Direct movement operations.

    // rotate functions in degrees.
    void rotateTo(float angle);
    void rotateBy(float angle, bool clockwise = true);
    // move functions to move to the designated position.
    void moveTo(Vector2 position);
    void moveBy(float distance);

    // Refresh renderer.
    void refreshRenderer(const Texture2D& texture);

    // Simple drawing.
    void draw();
};