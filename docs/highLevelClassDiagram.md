```mermaid
classDiagram
    direction TB
    class Drawable {
        <<Interface>>
        +draw(const Transform& transform)
    }

    class Shape {
        <<Interface>>
        +doesCollide(const Shape& other) bool
    }

    class Transform {
    }

    class MovementCore {
        +update(bool collides)
        +move()
    }

    MovementCore --> Edges : references

    class Player {
        +update()
    }
    
    Player *-- Drawable : owns
    Player *-- Shape : owns
    Player *-- Transform : owns
    Player *-- MovementCore : owns

    class Navmesh {
        +generateMesh(const Edges& edges)
    }

    Navmesh --> Edges : depends on

    class PathingCore {
        +generatePath(Vector2 targetPosition)
        +update()
    }

    PathingCore --> Navmesh : references

    class Agent {
        +update()
    }

    Agent *-- Drawable : owns
    Agent *-- Shape : owns
    Agent *-- Transform : owns
    Agent *-- PathingCore : owns

    class Obstacle {
    }

    Obstacle *-- Drawable : owns
    Obstacle *-- Shape : owns
    Obstacle *-- Transform : owns

    class Edges {
        +generateEdges()
    }

    class Background {
        +GenerateTexture(int, int, double)
    }

    Background --> WorldGen : depends on
    Background *-- Drawable : owns

    class WorldGen {
        +generate(uint32_t seed, double scale, double persistence)
    }

    WorldGen *-- Edges : owns
    WorldGen *-- Navmesh : owns
    
