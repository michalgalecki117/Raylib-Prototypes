#include <raylib.h>
#include <vector>

#include "player.h"
#include "geometry.h"
#include "navmesh.h"

#include "perlin.h"
#include "PerlinNoise.hpp"

// Main function called at launch.
int main() 
{
    // 1. Initialisation
    // -----------------

    // Window size set by default for simplicity.
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    // Vector2 used for passing to a few objects.
    Vector2 screenDimensions = { (float)screenWidth, (float)screenHeight };
    
    // Player object initialisation.
    Player player = Player({75,75}, 5, GOLD, 0, 90);
    
    // Creation of Box array to contain user created obstacles.
    std::vector<Box> spawnedBoxes;

    // Window initialisation.
    InitWindow(screenWidth, screenHeight, "Prototype");
    // TargetFPS to detatch fps from machine performance.
    SetTargetFPS(60);
    
    // Create world with threshold = 0.53 and cellSize = 15.
    // Based on a perlin noise of seed 1337, scale 0.01 and persistence 0.1.
    World world(screenDimensions, 1338, 0.02, 0.1, 0.44, 15);
    // Setting Cave color theme by default
    world.SetTheme(ColorTheme(7));
    // Creating the background texture from the world perlin noise.
    Texture2D backgroundTexture = world.GenerateTexture(screenWidth, screenHeight, world.GetThreshold());

    // Line vector to store world vertices for collision detection.
    std::vector<Line> borders = world.GetVertices();

    // Navmesh object generated from borders.
    Navmesh navmesh(borders);

    // Logic and input parameters for event handling.
    bool needsRefresh = false;
    float wheelMove;
    Vector2 mousePos;

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
        if (IsKeyDown(KEY_A))           { player.UpdateWithKeys(180); }
        if (IsKeyDown(KEY_W))           { player.UpdateWithKeys(270); }
        if (IsKeyDown(KEY_D))           { player.UpdateWithKeys(0); }
        if (IsKeyDown(KEY_S))           { player.UpdateWithKeys(90); }

        // World perlin scale control.
        if (IsKeyPressed(KEY_LEFT))     { world.SetScale(world.GetScale() - 0.005); needsRefresh = true; }
        if (IsKeyPressed(KEY_RIGHT))    { world.SetScale(world.GetScale() + 0.005); needsRefresh = true; }

        // World perlin seed control.
        if (IsKeyPressed(KEY_DOWN))     { world.SetSeed(world.GetSeed() - 1); needsRefresh = true; }
        if (IsKeyPressed(KEY_UP))       { world.SetSeed(world.GetSeed() + 1); needsRefresh = true; }
        
        // World perlin threshold control. (Also world vertices).
        if (IsKeyPressed(KEY_LEFT_BRACKET))        { world.SetThreshold(world.GetThreshold() - 0.01); needsRefresh = true; }
        if (IsKeyPressed(KEY_RIGHT_BRACKET))        { world.SetThreshold(world.GetThreshold() + 0.01); needsRefresh = true; }
        
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

        // 2.2 Updates and logic.
        // ---------------------

        // Persistence update based on mousewheel movement.
        if (wheelMove != 0.0f) 
        {
            // Update world persistence.
            world.SetPersistence(world.GetPersistence() + (wheelMove * 0.05));
            needsRefresh = true;
        }

        // Refresh world object in case of parameter change.
        if (needsRefresh) 
        {
            // Delete old texture and create new one, also refrech world vertices array.
            UnloadTexture(backgroundTexture);
            backgroundTexture = world.Refresh(screenDimensions); // Clean 1-argument call!

            borders = world.GetVertices();
            navmesh = Navmesh(borders);

            needsRefresh = false;
        }

        // Player object is the only object requiring continues unconditional updates. 
        player.update(spawnedBoxes, borders);


        // 2.3 Drawing
        // -----------

        BeginDrawing();

            // Background color in case nothing gets drawn.
            ClearBackground(BLACK);

            if (showbackground) 
            {
                // Draw background perlin noise based texture, if turned on.
                DrawTexturePro(
                    backgroundTexture,
                    (Rectangle){ 0, 0, screenDimensions.x, screenDimensions.y },          
                    (Rectangle){ 0, 0, screenDimensions.x, screenDimensions.y },    
                    (Vector2){ 0, 0 },                                              
                    0.0f,                                                           
                    WHITE                                                           
                );
            }

            // Draw world grid, if turned on.
            if (showgrid) { world.DrawGrid(); }

            // Draw navmesh from navmesh object, if turned on.
            if (shownavmesh) { navmesh.DrawNavmesh(); }

            // Draw vertices from world object, if turned on.
            if (showedges) { world.Draw(); }

            // Draw edge line points, if turned on.
            if (showpoints) { navmesh.DrawPoints(); }

            // Draw user spawned box obstacles.
            for (Box box : spawnedBoxes) { box.Draw(); }

            // Draw cursor location, if turned on.
            if (showcursor) 
            {
                DrawText(
                    TextFormat("%.f, %.f", mousePos.x, mousePos.y), mousePos.x + 7, mousePos.y - 13, 16, WHITE
                );
            }

            // Draw text explaining current parameters of perlin noise, if turned on.
            if (showinfo) 
            {
                DrawText(
                    TextFormat("Seed: %u | Scale: %.4f | Threshold: %.2f | Persistence: %.4f", 
                        world.GetSeed(), world.GetScale(), world.GetThreshold(), world.GetPersistence()),
                    20, 20, 20, WHITE
                );

                DrawText(
                    TextFormat("Grid (G) : %s\nEdges (E) : %s\nPoints (P) : %s\nNavmmesh (N) : %s\nCursor (C) : %s\nBackground (B) : %s",
                        (showgrid) ? "on" : "off", (showedges) ? "on" : "off",
                        (showpoints) ? "on" : "off", (shownavmesh) ? "on" : "off",
                        (showcursor) ? "on" : "off", (showbackground) ? "on" : "off"),
                     20, 450, 20, WHITE
                );

            }

            // Draw the player on top of everything else.
            player.Draw();
        
            EndDrawing();
    }
    

    // 3. Unloading and Exit.
    // ----------------------

    // Free up textures
    UnloadTexture(backgroundTexture);
    
    // Window close and program exit.
    CloseWindow();
    return 0;
}