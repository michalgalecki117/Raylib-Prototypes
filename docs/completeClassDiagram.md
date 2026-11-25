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
        -ObjectTransform transform
        +update()
    }
    
    Player *-- Drawable : owns
    Player *-- Shape : owns
    Player *-- ObjectTransform : owns
    Player *-- MovementCore : owns

    class Agent {
        -unique_ptr~Drawable~ renderer
        -Texture2D texture
        -unique_ptr~Shape~ collisionShape
        -PathingCore pathing
        -ObjectTransform transform
        +update()
    }

    Agent *-- Drawable : owns
    Agent *-- Shape : owns
    Agent *-- ObjectTransform : owns
    Agent *-- PathingCore : owns

    class ObjectTransform {
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

    class GameObject {
        -unique_ptr~Drawable~ renderer
        -unique_ptr~Shape~ collisionShape
        -ObjectTransform transform
        -bool collider
        -bool overrideTerrain
        +makeGameObject(collisionShape, transform...) GameObject
        +rotateTo(float angle)
        +rotateBy(float angle, bool clockwise)
        +moveTo(Vector2 position)
        +moveBy(float distance)
    }

    GameObject *-- Drawable : owns
    GameObject *-- Shape : owns
    GameObject *-- ObjectTransform : owns

    class WorldGen {
        -Perlin perlin
        -World world
        -map<float, Navmesh> meshes
        +makeNew(seed, scale, persistence, threshold, dimensions, cellSize)
        +reset(seed, scale, persistence, threshold)
        +getNavmesh(float key) const Navmesh&
    }

    WorldGen *-- Perlin : owns
    WorldGen *-- World : owns
    WorldGen *-- Navmesh : owns

    class Navmesh {
        -const World& world
        -vector~CDT::Triangle~ triangles
        -vector~CDT::V2d~double~~ points
        -int version
        -int lastSeenVersion
        -getVectorFromV2d(CDT::V2d<double> v2d) const Vector2
        -getV2dFromVector(Vector2 vector) const CDT::V2d<double>
        -generateMeshFromWorld()
        +makeNavmesh(const World& world) Navmesh
        +update()
    }

    Navmesh --> World : depends on

    class Perlin {        
        -siv::PerlinNoise m_perlin
        -uint32_t seed
        -double scale
        -double persistence
        -int version
        +sampleNoise(int, int) double
    }

    class World {
        -Perlin& perlin
        -vector~Vector2~ points
        -vector~pair~size_t,size_t~~ edges
        -float threshold
        -vector~GameObject~ obstacles
        -Vector2 dimensions
        -int cellSize
        -int version
        -int lastSeenVersion
        -interpolatePoints(p1, p2, v1, v2, dimensions) const Vector2
        -buildBorderSegments(pts, cornerA, cornerB, sortX)
        -addUniquePoint(Vector2 point, float epsilon) size_t
        -isInsideBorder(pt, minP, maxX, maxY) bool
        -generateEdgesFromNoise()
        +makeWorld(perlin, threshold, dimensions, cellSize) World
        +update()
    }

    World <-- Perlin : depend on
    World o-- GameObject : has

    class Drawable {
        <<Interface>>
        +draw(const ObjectTransform& transform)
    }

    class ShapeDrawer {
        -Color color
        -const Shape& shape 
        +draw(const ObjectTransform& transform)
    }

    ShapeDrawer --> Shape : references

    class TextureDrawer {
        -const Texture2d& texture
        +draw(const ObjectTransform& transform)
    }

    Drawable <|.. ShapeDrawer : implements
    Drawable <|.. TextureDrawer : implements

    class Shape {
        <<Interface>>
        static makeShape(shapePoints) unique_ptr<Shape>
    }

    class TCircle {
        -float radius
    }

    class TRectangle {
        -Vector2 middle
        -Vector2 size
    }

    class TLine {
        -Vector2 middle
        -Vector2 pos1
        -Vector2 pos2
    }

    class TTriangle {
        -Vector2 middle
        -Vector2 pos1
        -Vector2 pos2
        -Vector2 pos3
    }

    class TPolygon {
        -std::vector<CDT::V2d<double>> points
        -std::vector<CDT::Triangle> triangles
        -Vector2 origin
    }

    Shape <|.. TCircle : implements
    Shape <|.. TRectangle : implements
    Shape <|.. TPolygon : implements
    Shape <|.. TTriangle : implements
    Shape <|.. TLine : implements

    class TextureGenerator {
        -map<float, Color> pattern
        -const int width
        -const int height
        -getColorForNoise(double noise) const Color
        +static makeGenerator(pattern, dimensions) TextureGenerator
        +GenerateTexture() Texture2D
    }

    TextureGenerator --> Perlin : depends on