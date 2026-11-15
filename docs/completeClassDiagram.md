```mermaid
classDiagram
    direction TB
    class Drawable {
        <<Interface>>
        +draw(const Transform& transform)
    }

    class ShapeDrawer {
        -Color color
        -Shape& shape 
        +draw(const Transform& transform)
    }

    ShapeDrawer --> Shape : references

    class TextureDrawer {
        -Texture2d texture
        +draw(const Transform& transform)
    }

    Drawable <|.. ShapeDrawer : implements
    Drawable <|.. TextureDrawer : implements

    class Shape {
        <<Interface>>
        +doesCollide(const Shape& other) bool
        +doesCollideWithCircle(const Circle& other) bool
        +doesCollideWithRectangle(const Rectangle& other) bool
        +doesCollideWithLine(const Line& other) bool
        +doesCollideWithPolygon(const Polygon& other) bool
    }

    class Circle {
        -float radius
    }

    class Rectangle {
        -Vector2 size
    }

    class Line {
        -Vector2 startPos
        -Vector2 endPos
    }

    class Triangle {
        -Vector2 pos1
        -Vector2 pos2
        -Vector2 pos3
    }

    class Polygon {
        -vector~Vector2~ vertices
    }

    Shape <|.. Circle : implements
    Shape <|.. Rectangle : implements
    Shape <|.. Polygon : implements
    Shape <|.. Triangle : implements
    Shape <|.. Line : implements

    class Transform {
        -Vector2 position
        -float direction
        -float scale
    }

    class MovementCore {
        -Edges& edges
        -float speed
        -float direction
        +update(bool collides)
        +move()
    }

    MovementCore --> Edges : references

    class Player {
        -unique_ptr~Drawable~ renderer
        -unique_ptr~Shape~ collisionShape
        -MovementCore movement
        -Transform transform
        +update()
    }
    
    Player *-- Drawable : owns
    Player *-- Shape : owns
    Player *-- Transform : owns
    Player *-- MovementCore : owns

    class Navmesh {
        -vector~CDT::Triangle~ triangles
        -vector~CDT::V2d~double~~ vertices
        -Edges& edges
        +generateMesh(const Edges& edges)
    }

    Navmesh --> Edges : depends on

    class PathingCore {
        -float speed
        -float direction
        -Navmesh& navmesh
        +generatePath(Vector2 targetPosition)
        +update()
    }

    PathingCore --> Navmesh : references

    class Agent {
        -unique_ptr~Drawable~ renderer
        -unique_ptr~Shape~ collisionShape
        -PathingCore pathing
        -Transform transform
        +update()
    }

    Agent *-- Drawable : owns
    Agent *-- Shape : owns
    Agent *-- Transform : owns
    Agent *-- PathingCore : owns

    class Obstacle {
        -unique_ptr~Drawable~ renderer
        -unique_ptr~Shape~ collisionShape
        -Transform transform
    }

    Obstacle *-- Drawable : owns
    Obstacle *-- Shape : owns
    Obstacle *-- Transform : owns

    class Perlin {        
        -siv::PerlinNoise m_perlin
        -uint32_t seed
        -double scale
        -double persistence
        +sampleNoise(int, int) double
    }

    class Edges {
        -vector~Line~ edges
        -vector~Obstacle~ obstacles
        -Perlin& perlin
        +generateEdges()
    }

    Edges <-- Perlin : depend on

    class Background {
        -unique_ptr~Drawable~ renderer
        -Texture2D texture
        -Perlin& perlin
        -vector~float~ thresholds
        -vector~Color~ colors
        +GenerateTexture(int, int, double)
    }

    Background *-- Drawable : owns
    Background --> Perlin : depends on

    class WorldGen {
        -Perlin perlin
        -Edges edges
        -Navmesh navmesh
        +generate(uint32_t seed, double scale, double persistence)
    }

    WorldGen *-- Perlin : owns
    WorldGen *-- Edges : owns
    WorldGen *-- Navmesh : owns