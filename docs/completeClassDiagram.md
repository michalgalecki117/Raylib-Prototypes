```mermaid
---
config:
  layout: elk
  elk:
    mergeWorld: false
    nodePlacementStrategy: NETWORK_SIMPLEX
---
classDiagram
    direction TB
    class Player {
        -unique_ptr~Drawable~ renderer
        -Texture2D texture
        -unique_ptr~Shape~ collisionShape
        -MovementCore movement
        -Transform transform
        +update()
    }
    
    Player *-- Drawable : owns
    Player *-- Shape : owns
    Player *-- Transform : owns
    Player *-- MovementCore : owns

    class Agent {
        -unique_ptr~Drawable~ renderer
        -Texture2D texture
        -unique_ptr~Shape~ collisionShape
        -PathingCore pathing
        -Transform transform
        +update()
    }

    Agent *-- Drawable : owns
    Agent *-- Shape : owns
    Agent *-- Transform : owns
    Agent *-- PathingCore : owns

    class Transform {
        -Vector2 position
        -float rotation
        -float scale
    }

    class MovementCore {
        -World& edges
        -float speed
        -float direction
        +update(bool collides)
        +move()
    }

    MovementCore --> World : references

    class PathingCore {
        -float speed
        -float direction
        -Navmesh& navmesh
        +generatePath(Vector2 targetPosition)
        +update()
    }

    PathingCore --> Navmesh : references

    class Obstacle {
        -unique_ptr~Drawable~ renderer
        -unique_ptr~Shape~ collisionShape
        -Transform transform
        -bool collider
        -bool overrideTerrain
        +rotateTo(float angle)
        +rotateBy(float angle, bool clockwise)
        +moveTo(Vector2 position)
        +moveBy(float distance)
    }

    Obstacle *-- Drawable : owns
    Obstacle *-- Shape : owns
    Obstacle *-- Transform : owns

    class WorldGen {
        -Perlin perlin
        -World edges
        -Navmesh navmesh
        +generate(uint32_t seed, double scale, double persistence)
    }

    WorldGen *-- Perlin : owns
    WorldGen *-- World : owns
    WorldGen *-- Navmesh : owns

    class Navmesh {
        -vector~CDT::Triangle~ triangles
        -vector~CDT::V2d~double~~ vertices
        -World& edges
        +generateMesh(const World& edges)
    }

    Navmesh --> World : depends on

    class Perlin {        
        -siv::PerlinNoise m_perlin
        -uint32_t seed
        -double scale
        -double persistence
        +sampleNoise(int, int) double
    }

    class World {
        -vector~Line~ edges
        -vector~Obstacle~ obstacles
        -Perlin& perlin
        +generateWorld()
    }

    World <-- Perlin : depend on
    World *-- Obstacle : owns

    class Drawable {
        <<Interface>>
        +draw(const Transform& transform)
    }

    class ShapeDrawer {
        -Color color
        -const Shape& shape 
        +draw(const Transform& transform)
    }

    ShapeDrawer --> Shape : references

    class TextureDrawer {
        -const Texture2d texture
        +draw(const Transform& transform)
    }

    Drawable <|.. ShapeDrawer : implements
    Drawable <|.. TextureDrawer : implements

        class Shape {
        <<Interface>>
        static makeShape(shape specific parameters) unique_ptr<Shape>
    }

    class Circle {
        -float radius
    }

    class Rectangle {
        -Vector2 middle
        -Vector2 size
    }

    class Line {
        -Vector2 middle
        -Vector2 startPos
        -Vector2 endPos
    }

    class Triangle {
        -Vector2 middle
        -Vector2 pos1
        -Vector2 pos2
        -Vector2 pos3
    }

    class Polygon {
        -Vector2 origin
        -mesh::Mesh mesh
    }

    Shape <|.. Circle : implements
    Shape <|.. Rectangle : implements
    Shape <|.. Polygon : implements
    Shape <|.. Triangle : implements
    Shape <|.. Line : implements

    class TextureGenerator {
        -map<float, Color> pattern
        -const int width
        -const int height
        -getColorForNoise(souble noise) const Color
        +static makeGenerator(pattern, dimensions) TextureGenerator
        +GenerateTexture() Texture2D
    }

    TextureGenerator --> Perlin : depends on