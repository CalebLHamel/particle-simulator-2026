#include <iostream>
#include "raylib.h"
#include "particle.h"
#include "raymath.h"
#include "particle_type.h"

int main()
{
    /*
     * Example taken and modified from Raylib website.
     */

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Learning Things");

    Vector2 positionOffset = { (float)screenWidth/2, (float)screenHeight/2 };
    Vector2 textPositionOffset = { 0.0, 0.0 };
    
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    
    // Additional setup
    //--------------------------------------------------------------------------------------
    DefaultQualities test_qualities = DefaultQualities();
    test_qualities.SetQuality(QualityTypes::Mass, 10.0);
    test_qualities.SetQuality(QualityTypes::Charge, 5.0);
    
    ParticleType test_particles = ParticleType(15.0, test_qualities);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Vector2 new_position = Vector2 {(float)50*i + positionOffset.x, (float)50*j + positionOffset.y};
            Vector2 new_velocity = Vector2 {(float)(i-1)*j, (float)(j-1)*i};
            test_particles.addParticle(new_position, new_velocity);
        }
    }

    
    // Main game loop
    //--------------------------------------------------------------------------------------
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

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            int x = textPositionOffset.x;
            int y = textPositionOffset.y;
            DrawText("move the ball with arrow keys\nmove the text with wasd", 10+x, 10+y, 20, DARKGRAY);

            // For every test particle.
            for (int i = 0; i<9; i++) {
                Vector2 pos = test_particles.getPosition(i);            // Get the position of the particle.
                DrawCircleV(pos, test_particles.getRadius(), MAROON);   // Draw a circle at the particle's position.
                pos = Vector2Add(pos, test_particles.getVelocity(i));   // Get the particle's velocity.
                test_particles.setPosition(i, pos);                     // Use the velocity to update the particle's position.

                pos = test_particles.getPosition(i);
                Vector2 vel = test_particles.getVelocity(i);
                if (pos.x > screenWidth && vel.x > 0) {
                    vel.x *= -1;
                    test_particles.setVelocity(i, vel);
                } else if (pos.x < 0 && vel.x < 0) {
                    vel.x *= -1;
                    test_particles.setVelocity(i, vel);
                }
                if (pos.y > screenHeight && vel.y > 0) {
                    vel.y *= -1;
                    test_particles.setVelocity(i, vel);
                } else if (pos.y < 0 && vel.y < 0) {
                    vel.y *= -1;
                    test_particles.setVelocity(i, vel);
                }
            }

            for (int i = 0; i<9; i++) {
                for (int j=i+1; j<9; j++) {
                    Vector2 pos1 = test_particles.getPosition(i);
                    Vector2 pos2 = test_particles.getPosition(j);
                    float m = test_particles.getQuality(QualityTypes::Mass);

                    Vector2 diff = Vector2Subtract(pos2, pos1);
                    float diff_magnitude_squared = Vector2LengthSqr(diff);
                    if (diff_magnitude_squared < 30) {
                        continue;
                    }
                    
                    float acc_magnitude = (m*m) / diff_magnitude_squared;
                    Vector2 acc = Vector2Scale(Vector2Normalize(diff), 3.0*acc_magnitude / m);

                    Vector2 vel1 = test_particles.getVelocity(i);
                    Vector2 vel2 = test_particles.getVelocity(j);
                    
                    vel1 = Vector2Add(vel1, acc);
                    vel2 = Vector2Subtract(vel2, acc);
                    test_particles.setVelocity(i, vel1);
                    test_particles.setVelocity(j, vel2);
                }
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}