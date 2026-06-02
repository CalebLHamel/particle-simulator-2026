#include <iostream>
#include "raylib.h"
#include "particle.h"
#include "raymath.h"

int main()
{
    /*
     * Example taken from Raylib website.
     */

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Learning Things");

    Vector2 positionOffset = { (float)screenWidth/2, (float)screenHeight/2 };
    Vector2 textPositionOffset = { 0.0, 0.0 };

    Particle* particles[9];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            particles[i*3+j] = new Particle(20.0, Vector2 {(float)50*i, (float)50*j});
        }
    }

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)) positionOffset.x += 4.0f;
        if (IsKeyDown(KEY_LEFT)) positionOffset.x -= 4.0f;
        if (IsKeyDown(KEY_UP)) positionOffset.y -= 4.0f;
        if (IsKeyDown(KEY_DOWN)) positionOffset.y += 4.0f;

        if (IsKeyDown(KEY_D)) textPositionOffset.x += 4.0f;
        if (IsKeyDown(KEY_A)) textPositionOffset.x -= 4.0f;
        if (IsKeyDown(KEY_W)) textPositionOffset.y -= 4.0f;
        if (IsKeyDown(KEY_S)) textPositionOffset.y += 4.0f;

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            int x = textPositionOffset.x;
            int y = textPositionOffset.y;
            DrawText("move the ball with arrow keys\nmove the text with wasd", 10+x, 10+y, 20, DARKGRAY);

            for (int i = 0; i<9; i++) {
                Particle* p = particles[i];
                Vector2 v = Vector2Add(p->getPosition(), positionOffset);
                DrawCircleV(v, p->getRadius(), MAROON);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    for (int i=0; i<9; i++) {
        delete particles[i];
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}