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

    // Color to be used as background in abscence of drawn textures.
    const Color darkGreen = {20, 160, 133, 255};

    // Window size set by default for simplicity.
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    // Vector2 used for passing to a few objects.
    Vector2 screenDimensions = { (float)screenWidth, (float)screenHeight };
    
    // Player object initialisation.
    Player player = Player({100,100}, 15, RED, 5, 90);
    
    // Creation of Box array to contain user created obstacles.
    std::vector<Box> spawnedBoxes;

    // Window initialisation.
    InitWindow(screenWidth, screenHeight, "Vertices based map and box colisions test.");
    // TargetFPS to detatch fps from machine performance.
    SetTargetFPS(60);
    
    // Create world with threshold = 0.53 and cellSize = 15.
    // Based on a perlin noise of seed 1337, scale 0.01 and persistence 0.1.
    World world(screenDimensions, 1337, 0.01, 0.1, 0.53, 15);
    // Setting Cave color theme by default
    world.SetTheme(ColorTheme(7));
    // Creating the background texture from the world perlin noise.
    Texture2D backgroundTexture = world.GenerateTexture(screenWidth, screenHeight, world.GetThreshold());

    // Logic and input parameters for event handling.
    bool needsRefresh = false;
    float wheelMove;
    bool showvertices = false;
    
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
        if (IsKeyPressed(KEY_T))        { world.SetThreshold(world.GetThreshold() + 0.01); needsRefresh = true; }
        if (IsKeyPressed(KEY_G))        { world.SetThreshold(world.GetThreshold() - 0.01); needsRefresh = true; }
        
        // World perlin visualisation theme cycling.
        if (IsKeyPressed(KEY_SPACE))    { world.CycleTheme(); needsRefresh = true; }

        // World vertices array visualisation control.
        if (IsKeyPressed(KEY_V))        { showvertices = !showvertices; }

        // Mouse wheel movement for world perlin persistence control.
        wheelMove = GetMouseWheelMove();
        
        // Obstacle spawning.
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
        {
            // Create new box at mouse position.
            Vector2 boxPos = GetMousePosition();
            // Substract half the box size to put its center at mouse pointer.
            boxPos.x -= 20.0f;
            boxPos.y -= 20.0f;
            // Add it to obstacle array.
            spawnedBoxes.push_back(Box(boxPos, {40.0f, 40.0f}, BLACK));
        }        
        

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

            needsRefresh = false;
        }

        // Player object is the only object requiring continues unconditional updates. 
        player.update(spawnedBoxes);


        // 2.3 Drawing
        // -----------

        BeginDrawing();

            // Background color in case nothing gets drawn.
            ClearBackground(darkGreen);

            // Draw background perlin noise based texture.
            DrawTexturePro(
                backgroundTexture,
                (Rectangle){ 0, 0, screenDimensions.x, screenDimensions.y },          
                (Rectangle){ 0, 0, screenDimensions.x, screenDimensions.y },    
                (Vector2){ 0, 0 },                                              
                0.0f,                                                           
                WHITE                                                           
            );

            // Draw user spawned box obstacles.
            for (Box box : spawnedBoxes) { box.Draw(); }

            // Draw vertices from world object, if turned on.
            if (showvertices) { world.Draw(); }

            // Draw text explaining current parameters of perlin noise.
            DrawText(
                TextFormat("Theme: %s | Seed: %u | Scale: %.4f | Threshold: %.2f", 
                    world.GetThemeName(), world.GetSeed(), world.GetScale(), world.GetThreshold()),
                20, 20, 16, RED
            );

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