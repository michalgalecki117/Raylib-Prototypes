```mermaid
---
config:
  layout: elk
  elk:
    mergeEdges: false
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
        -Texture2D texture
        -unique_ptr~Shape~ collisionShape
        -Transform transform
    }

    Obstacle *-- Drawable : owns
    Obstacle *-- Shape : owns
    Obstacle *-- Transform : owns

    class WorldGen {
        -Perlin perlin
        -Edges edges
        -Navmesh navmesh
        +generate(uint32_t seed, double scale, double persistence)
    }

    WorldGen *-- Perlin : owns
    WorldGen *-- Edges : owns
    WorldGen *-- Navmesh : owns

    class Navmesh {
        -vector~CDT::Triangle~ triangles
        -vector~CDT::V2d~double~~ vertices
        -Edges& edges
        +generateMesh(const Edges& edges)
    }

    Navmesh --> Edges : depends on

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
    }

    class Circle {
        -float radius
    }

    class Rectangle {
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

    class Background {
        -unique_ptr~Drawable~ renderer
        -Texture2D texture
        -vector~float~ thresholds
        -vector~Color~ colors
        +GenerateTexture(int, int, double)
    }

    Background *-- Drawable : owns
    Background --> Perlin : depends on