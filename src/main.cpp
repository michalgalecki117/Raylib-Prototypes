#include "raylib.h"
#include <vector>
#include <memory>

#include "worldgen.h"
#include "rendering.h"
#include "transform.h"
#include "gameObject.h"
#include "textureGenerator.h"
#include "player.h"

// Main function called at launch.
int main() 
{
    // 1. Initialisation
    // -----------------

    // Window size set by default for simplicity.
    constexpr int screenWidth = 1000;
    constexpr int screenHeight = 750;
    // Vector2 used for passing to a few objects.
    Vector2 screenDimensions = { (float)screenWidth, (float)screenHeight };
    
    // Player object initialisation.
    std::unique_ptr<Shape> playerShape = Shape::makeCircle(5);
    ObjectTransform playerTransform = ObjectTransform((Vector2){75.0f,75.0f}, 0, 1);
    std::unique_ptr<Player> player = Player::makePlayer(std::move(playerShape), playerTransform, GOLD);
    
    const Drawable& playerRenderer = player->getRenderer();

    // Window initialisation.
    InitWindow(screenWidth, screenHeight, "Prototype");
    // TargetFPS to detatch fps from machine performance.
    SetTargetFPS(60);
    
    // Initialise world generation pipeline and storage.
    WorldGen worldGen = WorldGen::makeNew(1338, 0.02, 0.01, 0.44, screenDimensions, 15);

    // Transform component of the background texture itself.
    ObjectTransform backgroundTransform = ObjectTransform((Vector2){500.0f,375.0f}, 0, 1);
    // Creating the background texture from the world perlin noise.
    std::vector<float> thresholds = {0.43f, 0.44f, 0.45f};
    std::vector<Color> colors = {{ 30, 30, 40, 255 }, { 80, 80, 100, 255 }, { 15, 15, 20, 255 }};
    TextureGenerator generator = TextureGenerator::makeGenerator(
        thresholds,
        colors,
        screenDimensions
    );
    Texture2D newBackgroundTexture = generator.generateTexture(worldGen.getPerlin());

    // Creating renderer for the background textures.
    std::unique_ptr<Drawable> backgroundRenderer = Drawable::makeDrawer(newBackgroundTexture);

    // Logic and input parameters for event handling.
    bool needsRefresh = false;
    // For redrawing the world with a different edge detection threshold.
    // This is purely for visualisation.
    float thresholdOffset = 0.0f;

    // Positional arguments.
    float wheelMove;
    Vector2 mousePos;

    // Logical arguments.
    bool showedges = false;
    bool shownavmesh = false;
    bool showpoints = false;
    bool showgrid = false;
    bool showcursor = false;
    // Only things displayed by default.
    bool showbackground = true;
    bool showinfo = true;
    
    // 2. Window Main loop
    // -------------------

    while (!WindowShouldClose())
    {

        // 2.1 Event Catching and Handling
        // --------------------------------

        // Player movement.
        float moveX = 0.0f;
        float moveY = 0.0f;
        if (IsKeyDown(KEY_A)) moveX -= 1.0f;
        if (IsKeyDown(KEY_D)) moveX += 1.0f;
        if (IsKeyDown(KEY_W)) moveY -= 1.0f;
        if (IsKeyDown(KEY_S)) moveY += 1.0f;

        // World perlin scale control.
        if (IsKeyPressed(KEY_LEFT))     
        { 
            // Any change to base perlin noise is an automatic world generation reset.
            // We have to grab all of the base values and reset with the single desired change.
            // This is for testing purposes as usually one would directly reset world generation,
            // only if the actual game world needs changing. 
            // Example : loading new game, or loading a new area possibly.
            uint32_t tmpSeed = worldGen.getPerlin().getSeed();
            double tmpScale = worldGen.getPerlin().getScale();
            double tmpPer = worldGen.getPerlin().getPersistence();
            float tmpThresh = worldGen.getWorld().getThreshold();

            // World generation pipeline reset.
            worldGen.reset(tmpSeed, tmpScale - 0.005, tmpPer, tmpThresh);
            needsRefresh = true; 
        }
        if (IsKeyPressed(KEY_RIGHT))
        { 
            // Any change to base perlin noise is an automatic world generation reset.
            // We have to grab all of the base values and reset with the single desired change.
            // This is for testing purposes as usually one would directly reset world generation,
            // only if the actual game world needs changing. 
            // Example : loading new game, or loading a new area possibly.
            uint32_t tmpSeed = worldGen.getPerlin().getSeed();
            double tmpScale = worldGen.getPerlin().getScale();
            double tmpPer = worldGen.getPerlin().getPersistence();
            float tmpThresh = worldGen.getWorld().getThreshold();

            // World generation pipeline reset.
            worldGen.reset(tmpSeed, tmpScale + 0.005, tmpPer, tmpThresh);
            needsRefresh = true; 
        }

        // World perlin seed control.
        if (IsKeyPressed(KEY_DOWN))
        { 
            // Any change to base perlin noise is an automatic world generation reset.
            // We have to grab all of the base values and reset with the single desired change.
            // This is for testing purposes as usually one would directly reset world generation,
            // only if the actual game world needs changing. 
            // Example : loading new game, or loading a new area possibly.
            uint32_t tmpSeed = worldGen.getPerlin().getSeed();
            double tmpScale = worldGen.getPerlin().getScale();
            double tmpPer = worldGen.getPerlin().getPersistence();
            float tmpThresh = worldGen.getWorld().getThreshold();

            // World generation pipeline reset.
            worldGen.reset(tmpSeed - 1, tmpScale, tmpPer, tmpThresh);
            needsRefresh = true; 
        }
        if (IsKeyPressed(KEY_UP))
        { 
            // Any change to base perlin noise is an automatic world generation reset.
            // We have to grab all of the base values and reset with the single desired change.
            // This is for testing purposes as usually one would directly reset world generation,
            // only if the actual game world needs changing. 
            // Example : loading new game, or loading a new area possibly.
            uint32_t tmpSeed = worldGen.getPerlin().getSeed();
            double tmpScale = worldGen.getPerlin().getScale();
            double tmpPer = worldGen.getPerlin().getPersistence();
            float tmpThresh = worldGen.getWorld().getThreshold();

            // World generation pipeline reset.
            worldGen.reset(tmpSeed + 1, tmpScale, tmpPer, tmpThresh);
            needsRefresh = true; 
        }
        
        // World perlin threshold control. (Also world vertices).
        if (IsKeyPressed(KEY_LEFT_BRACKET))
        { 
            // Any change to base perlin noise is an automatic world generation reset.
            // We have to grab all of the base values and reset with the single desired change.
            // This is for testing purposes as usually one would directly reset world generation,
            // only if the actual game world needs changing. 
            // Example : loading new game, or loading a new area possibly.
            uint32_t tmpSeed = worldGen.getPerlin().getSeed();
            double tmpScale = worldGen.getPerlin().getScale();
            double tmpPer = worldGen.getPerlin().getPersistence();
            float tmpThresh = worldGen.getWorld().getThreshold();

            // World generation pipeline reset.
            worldGen.reset(tmpSeed, tmpScale, tmpPer, tmpThresh - 0.01);
            thresholdOffset -= 0.01;
            needsRefresh = true; 
        }
        if (IsKeyPressed(KEY_RIGHT_BRACKET))
        { 
            // Any change to base perlin noise is an automatic world generation reset.
            // We have to grab all of the base values and reset with the single desired change.
            // This is for testing purposes as usually one would directly reset world generation,
            // only if the actual game world needs changing. 
            // Example : loading new game, or loading a new area possibly.
            uint32_t tmpSeed = worldGen.getPerlin().getSeed();
            double tmpScale = worldGen.getPerlin().getScale();
            double tmpPer = worldGen.getPerlin().getPersistence();
            float tmpThresh = worldGen.getWorld().getThreshold();

            // World generation pipeline reset.
            worldGen.reset(tmpSeed, tmpScale, tmpPer, tmpThresh + 0.01);
            thresholdOffset += 0.01;
            needsRefresh = true; 
        }
        
        // World edges array visualisation control.
        if (IsKeyPressed(KEY_E))        { showedges = !showedges; }

        // Navmesh visualisation control.
        if (IsKeyPressed(KEY_N))        { shownavmesh = !shownavmesh; }

        // Edge line end points visualisation control.
        if (IsKeyPressed(KEY_P))        { showpoints = !showpoints; }

        // Grid of cellSize cells visualisation control.
        if (IsKeyPressed(KEY_G))        { showgrid = !showgrid; }

        // Grid of cellSize cells visualisation control.
        if (IsKeyPressed(KEY_B))        { showbackground = !showbackground; }

        // Control display of information text (about the perlin noise).
        if (IsKeyPressed(KEY_I))        { showinfo = !showinfo; }

        // Control display of cursor location.
        if (IsKeyPressed(KEY_C))        { showcursor = !showcursor; }

        // Mouse wheel movement for world perlin persistence control.
        wheelMove = GetMouseWheelMove();

        // Mouse position for cursor coordinate display.
        mousePos = GetMousePosition();

        // Persistence update based on mousewheel movement.
        if (wheelMove != 0.0f) 
        { 
            // Any change to base perlin noise is an automatic world generation reset.
            // We have to grab all of the base values and reset with the single desired change.
            // This is for testing purposes as usually one would directly reset world generation,
            // only if the actual game world needs changing. 
            // Example : loading new game, or loading a new area possibly.
            uint32_t tmpSeed = worldGen.getPerlin().getSeed();
            double tmpScale = worldGen.getPerlin().getScale();
            double tmpPer = worldGen.getPerlin().getPersistence();
            float tmpThresh = worldGen.getWorld().getThreshold();

            // World generation pipeline reset.
            worldGen.reset(tmpSeed, tmpScale, tmpPer + (wheelMove * 0.02), tmpThresh);
            needsRefresh = true; 
        }

        // 2.2 Updates and logic.
        // ---------------------

        // Refresh world object in case of parameter change.
        if (needsRefresh) 
        {
            // Delete old texture and create new one, also refrech world vertices array.
            UnloadTexture(newBackgroundTexture);

            std::vector<float> shiftedThresholds = thresholds;
            for (size_t i = 0; i < shiftedThresholds.size(); i++) 
            {
                shiftedThresholds[i] += thresholdOffset;
            }
            generator = TextureGenerator::makeGenerator(shiftedThresholds, colors, screenDimensions);
            newBackgroundTexture = generator.generateTexture(worldGen.getPerlin());

            backgroundRenderer = Drawable::makeDrawer(newBackgroundTexture);

            needsRefresh = false;
        }

        if (moveX != 0.0f || moveY != 0.0f)
            {
                float angleDeg = std::atan2(moveY, moveX) * (180.0f / PI);
                player->setTargetDirectionDegrees(angleDeg);
                player->setSpeed(2.0f);
            }

        // Player object is the only object requiring continues unconditional updates. 
        player->update();


        // 2.3 Drawing
        // -----------

        BeginDrawing();

            // Background color in case nothing gets drawn.
            ClearBackground(BLACK);

            if (showbackground) 
            {
                
                // Draw background perlin noise based texture, if turned on.
                backgroundRenderer->draw(backgroundTransform);
            }

            // Draw world grid, if turned on.
            if (showgrid) 
            { 
                // Purely for visualisation, create grid from cellSize to demonstrate function of Marching Squares.
                for (float x = 0; x < screenDimensions.x; x += 15) { DrawLineV({x, 0}, {x, screenDimensions.y}, {100,100,100,200}); }
                for (float y = 0; y < screenDimensions.y; y += 15) { DrawLineV({0, y}, {screenDimensions.x, y}, {100,100,100,200}); }
            }

            // Draw navmesh from navmesh object, if turned on.
            if (shownavmesh) 
            { 
                const Navmesh& mesh = worldGen.getNavmesh(0.0f);
                const std::vector<CDT::V2d<double>>& meshPoints = mesh.getPoints();
                const std::vector<CDT::Triangle>& meshTriangles = mesh.getTriangles();

                // Gets every CDT triangle in navmesh.
                for (const CDT::Triangle &t : meshTriangles)
                {
                    // Get all 3 indexes of the triangle points.
                    size_t i0 = t.vertices[0];
                    size_t i1 = t.vertices[1];
                    size_t i2 = t.vertices[2];

                    // Get the actual V2d point from points based on index.
                    const CDT::V2d<double> pt0 = meshPoints[i0];
                    const CDT::V2d<double> pt1 = meshPoints[i1];
                    const CDT::V2d<double> pt2 = meshPoints[i2];

                    // Convert V2d point to Vector2 point to allow drawing.
                    Vector2 v0 = {static_cast<float>(pt0.x), static_cast<float>(pt0.y)};
                    Vector2 v1 = {static_cast<float>(pt1.x), static_cast<float>(pt1.y)};
                    Vector2 v2 = {static_cast<float>(pt2.x), static_cast<float>(pt2.y)};

                    // Draw actual triangle overlay.
                    DrawTriangle(v0, v2, v1, {50, 100, 50, 50});
                    // Draw triangle edge lines to differentiate between triangles.
                    DrawTriangleLines(v0, v2, v1, {100, 200, 100, 100});
                }

            }

            // Draw vertices from world object, if turned on.
            if (showedges) 
            {
                const std::vector<Vector2>& points = worldGen.getWorld().getUniquePoints();
                const std::vector<std::pair<size_t, size_t>>& edges = worldGen.getWorld().getEdges();

                for (const std::pair<size_t, size_t> edge : edges) 
                {
                    DrawLineV(points[edge.first], points[edge.second], RED);
                }
            }

            // Draw edge line points, if turned on.
            if (showpoints) 
            { 
                const std::vector<Vector2>& points = worldGen.getWorld().getUniquePoints();
                for (const Vector2 point : points) { DrawCircleV(point, 2, BLUE); } 
            }

            // Draw cursor location, if turned on.
            if (showcursor) 
            {
                DrawText(
                    TextFormat("%.f, %.f, %.2f", mousePos.x, mousePos.y, worldGen.getPerlin().sampleNoise(mousePos.x, mousePos.y)), mousePos.x + 7, mousePos.y - 13, 16, WHITE
                );
            }

            // Draw text explaining current parameters of perlin noise, if turned on.
            if (showinfo) 
            {
                DrawText(
                    TextFormat("Seed: %u | Scale: %.3f | Persistence: %.2f | Threshold: %.2f", 
                        worldGen.getPerlin().getSeed(), worldGen.getPerlin().getScale(), 
                        worldGen.getPerlin().getPersistence(), worldGen.getWorld().getThreshold()),
                    20, 20, 20, WHITE
                );

                DrawText(
                    TextFormat("Grid (G) : %s\nEdges (E) : %s\nPoints (P) : %s\nNavmmesh (N) : %s\nCursor (C) : %s\nBackground (B) : %s",
                        (showgrid) ? "on" : "off", (showedges) ? "on" : "off",
                        (showpoints) ? "on" : "off", (shownavmesh) ? "on" : "off",
                        (showcursor) ? "on" : "off", (showbackground) ? "on" : "off"),
                     20, 600, 20, WHITE
                );

            }

            // Draw the player on top of everything else.
            playerRenderer.draw(player->getTransform());
            // player.Draw();
        
            EndDrawing();
    }
    
    // 3. Unloading and Exit.
    // ----------------------

    // Free up textures
    UnloadTexture(newBackgroundTexture);
    
    // Window close and program exit.
    CloseWindow();
    return 0;
}