#pragma once
#include <raylib.h>
#include <memory>

#include "transform.h"
#include "shapes.h"

class Drawable // Drawable Interface
{
public:
    // Purely virtual function for renderers to inherit.
    // Pass a constant reference to the drawable object's transform,
    // ie position, rotation and so on. Then draw the object.
    virtual void draw(const ObjectTransform& transform) = 0;
    // Set default destructor just in case.
    virtual ~Drawable() = default;

    static std::unique_ptr<Drawable> makeDrawer(const Color& color, const Shape& shape);
    static std::unique_ptr<Drawable> makeDrawer(const Texture2D& texture);
};

// Implementation of Drawable interface on simple shapes.
class ShapeDrawer : public Drawable 
{
private:
    // Shape drawer holds its assigned color,
    // but only the reference of it's target's shape.
    // That way if the shape changes, the drawing will update aswell.
    Color color;
    const Shape& shape;

    // Keeping the constructor private.
    // Only the interface factory method can create a new ShapeDrawer.
    ShapeDrawer(const Color& color, const Shape& shape)
        : color(color), shape(shape) {} 

    // Permit access to constructor only to Drawable factory.
    friend std::unique_ptr<Drawable> Drawable::makeDrawer(const Color& color, const Shape& shape);

public:
    // The only job of the ShapeDrawer is to draw the correct shape,
    // in the correct way. As such, all it needs for its draw function,
    // is the position, rotation and scale of the object it's drawing.
    void draw(const ObjectTransform& transform) override;
};

// Implementation of Drawable interface on 2d textures.
class TextureDrawer : public Drawable 
{
private:
    // Texture drawer only holds the referebce to the target's texture.
    // That way if the texture changes, the drawing will update aswell.
    const Texture2D& texture;

    // Keeping the constructor private.
    // Only the interface factory method can create a new TextureDrawer.
    TextureDrawer(const Texture2D& texture)
        : texture(texture) {}

    // Permit access to constructor only to Drawable factory.
    friend std::unique_ptr<Drawable> Drawable::makeDrawer(const Texture2D& texture);
public:
    // The only job of the TextureDrawer is to draw the correct texture,
    // in the correct way. As such, all it needs for its draw function,
    // is the position, rotation and scale of the object it's drawing.
    void draw(const ObjectTransform& transform) override;
};