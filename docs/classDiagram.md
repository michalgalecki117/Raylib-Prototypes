```mermaid
classDiagram
    class GameObject {
        <<abstract>>
        #Vector2 position
        #Color color
        +GameObject(Vector2, Color)
        +~GameObject()*
        +Draw()*
    }

    class Line {
        -Vector2 endPos
        +Line(Vector2, Vector2, Color)
        +~Line()*
        +Draw()
    }

    class Box {
        -Vector2 size
        +Box(Vector2, Vector2, Color)
        +~Box()*
        +GetRectangle() Rectangle
        +Draw()
    }
    
    class Circle {
        -int radius
        +Circle(Vector2, int, Color)
        +~Circle()*
        +Draw()
    }

    GameObject <|-- Line : inherits
    GameObject <|-- Box : inherits
    GameObject <|-- Circle : inherits

    class Player {
        -float speed
        -float direction
        +Player(Vector2, float, Color, float, float)
        +update(vector~Box~, vector~Line~)
        +UpdateWithKeys(int)
    }

    Circle <|-- Player : inherits

        class ColorTheme {
        <<enumeration>>
        Space
        Forest
        Fire
        Blood
        Monochrome
        Ocean
        Desert
        Cave
        Count
    }

    class Perlin {
        -siv::PerlinNoise m_perlin
        -uint32_t seed
        -double scacle
        -double persistence
        -ColorTheme m_theme
        +Perlin(uint32_t, double, double)
        +GetThemeName()
        +SampleNoise(int, int) double
        +GenerateTexture(int, int, double) Texture2D
        +CycleTheme()
    }

    ColorTheme <-- Perlin : has

    class World {
        -vector~Line~ lines
        -double threshold
        -int cellSize
        +World(Vector2, unint32_t, double, double, double, int)
        +GeneratVertices(Vector2) // Edges not vertices
        +Refresh(Vector2) Texture2D
        +update()
        +Draw()
        +DrawGrid() 
    }

    GameObject <|-- World : inherits
    Perlin <|-- World : inherits

    class Navmesh {
        -vector~CDT::Triangle~ triangles
        -vector~CDT::V2d~ points
        -vector~Vector2~ uniquePoints
        +Navmesh(vector~Line~)
        +DrawPoints()
        +DrawNavmesh()
    }