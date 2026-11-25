#include "rendering.h"
#include <memory>
#include <vector>
#include <cmath>
#include "CDT/CDT.h"

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
void TextureDrawer::draw(const ObjectTransform& transform) const
{
    const Vector2& position = transform.getPosition();
    const float& rotation = transform.getRotation();
    const float& scale = transform.getScale();

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
void ShapeDrawer::draw(const ObjectTransform& transform) const
{
    const Vector2& position = transform.getPosition();
    const float& rotation = transform.getRotation();
    const float& scale = transform.getScale();

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
        const std::vector<CDT::V2d<double>>& points = polygon->getPoints();
        const std::vector<CDT::Triangle>& triangles = polygon->getTriangles();
        middle = polygon->getOrigin();

        // Gets every CDT triangle in mesh.
        for (const CDT::Triangle& t : triangles) 
        {
            // Get all 3 indexes of the triangle points.
            size_t i0 = t.vertices[0];
            size_t i1 = t.vertices[1];
            size_t i2 = t.vertices[2];
    
            // Get the actual V2d point from points based on index.
            const CDT::V2d<double> pt0 = points[i0];
            const CDT::V2d<double> pt1 = points[i1];
            const CDT::V2d<double> pt2 = points[i2];

            // Convert V2d point to Vector2 point to allow drawing.
            Vector2 v0 = translateVertex({ static_cast<float>(pt0.x), static_cast<float>(pt0.y) });
            Vector2 v1 = translateVertex({ static_cast<float>(pt1.x), static_cast<float>(pt1.y) });
            Vector2 v2 = translateVertex({ static_cast<float>(pt2.x), static_cast<float>(pt2.y) });

            // Draw actual triangle overlay.
            DrawTriangle(v0, v1, v2, color);
        }
    }
    

}