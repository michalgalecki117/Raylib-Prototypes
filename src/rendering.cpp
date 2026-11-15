#include "rendering.h"
#include <memory>
#include <vector>
#include <cmath>

// Factory function of Drawable interface. 
// It makes the ShapeDrawer when provided a color and shape.
std::unique_ptr<Drawable> Drawable::makeDrawer(const Color& color, const Shape& shape) 
{
    return std::unique_ptr<Drawable>(new ShapeDrawer(color, shape));
}

// Factory function of Drawable interface. 
// It makes the TextureDrawer when provided a texture.
std::unique_ptr<Drawable> Drawable::makeDrawer(const Texture2D& texture)
{
    return std::unique_ptr<Drawable>(new TextureDrawer(texture));
}

// draw function of the TextureDrawer class.
// Draws its loaded texture at the position, rotation and scale indicated by transform.
void TextureDrawer::draw(const ObjectTransform& transform) 
{
    Vector2 position = transform.getPosition();
    float rotation = transform.getRotation();
    float scale = transform.getScale();

    float width = static_cast<float>(texture.width) * scale;
    float height = static_cast<float>(texture.height) * scale;

    DrawTexturePro(
        texture,
        (Rectangle){ 0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)},          
        (Rectangle){ position.x, position.y, width, height },    
        (Vector2){ width * 0.5f, height * 0.5f },                                              
        rotation,                                                           
        WHITE                                                           
    );

}

// draw function of the ShapeDrawer class.
// Draws its shape in its given color at the position, rotation and scale indicated by transform.
void ShapeDrawer::draw(const ObjectTransform& transform) 
{
    Vector2 position = transform.getPosition();
    float rotation = transform.getRotation();
    float scale = transform.getScale();

    float radians = rotation * DEG2RAD;
    float cosA = cos(radians);
    float sinA = sin(radians);

    Vector2 middle;

    auto translateVertex = [&](Vector2 vertex) 
        {
            float relX = vertex.x - middle.x;
            float relY = vertex.y - middle.y;

            float scaledX = relX * scale;
            float scaledY = relY * scale;

            float rotX = (scaledX * cosA) + (scaledY * sinA);
            float rotY = (scaledX * sinA) - (scaledY * cosA);

            return (Vector2){rotX + position.x, rotY + position.y};
        };


    if (const auto* circle = dynamic_cast<const TCircle*>(&shape)) 
    {
        DrawCircleV(position, circle->getRadius(), color);
    }

    if (const auto* rectangle = dynamic_cast<const TRectangle*>(&shape)) 
    {
        Vector2 size = rectangle->getSize();
        
        float width = size.x * scale;
        float height = size.y * scale;

        Rectangle rect = {position.x, position.y, width, height};

        DrawRectanglePro(
            rect,
            (Vector2){size.x * 0.5f * scale, size.y * 0.5f * scale},
            rotation,
            color
        );
    }

    if (const auto* triangle = dynamic_cast<const TTriangle*>(&shape)) 
    {
        std::vector<Vector2> pos = triangle->getVertices();
        middle = triangle->getMiddle();
        Vector2 pos1 = pos[0];
        Vector2 pos2 = pos[1];
        Vector2 pos3 = pos[2]; 

        Vector2 realPos1 = translateVertex(pos1);
        Vector2 realPos2 = translateVertex(pos2);
        Vector2 realPos3 = translateVertex(pos3);

        DrawTriangle(realPos1, realPos2, realPos3,color);

    }

    if (const auto* line = dynamic_cast<const TLine*>(&shape)) 
    {
        std::vector<Vector2> pos = line->getVertices();
        middle = line->getMiddle();
        Vector2 pos1 = pos[0];
        Vector2 pos2 = pos[1]; 

        Vector2 realPos1 = translateVertex(pos1);
        Vector2 realPos2 = translateVertex(pos2);

        DrawLineV(realPos1, realPos2,color);

    }

    if (const auto* polygon = dynamic_cast<const TPolygon*>(&shape)) 
    {
        std::vector<Vector2> pos = polygon->getVertices();
        middle = polygon->getOrigin();
        int sides = polygon->getSides();

        std::vector<Vector2> realPositions;
        realPositions.reserve(sides + 1);
        realPositions.push_back(translateVertex(middle));
        
        for (int i = 0; i < sides; i++) 
        {
            realPositions.push_back(translateVertex(pos[i]));
        }

        DrawTriangleFan(realPositions.data(), sides, color);

    }
    

}