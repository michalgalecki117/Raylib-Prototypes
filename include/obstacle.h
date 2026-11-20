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

    // Constructor with variables kept private for safe obstacle building.
    Obstacle(std::unique_ptr<Drawable> renderer, std::unique_ptr<Shape> collisionShape, ObjectTransform transform);

public:
    // Public Obstacle factories for safe building.
    static Obstacle makeObstacle(std::unique_ptr<Shape> collisionShape, ObjectTransform transform, Color color = BLACK);
    static Obstacle makeObstacle(const Texture2D& texture, std::unique_ptr<Shape> collisionShape, ObjectTransform transform);

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