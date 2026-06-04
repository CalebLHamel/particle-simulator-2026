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
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Learning Things");

    Vector2 positionOffset = { (float)screenWidth/2, (float)screenHeight/2 };
    Vector2 textPositionOffset = { 0.0, 0.0 };
    
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    
    // Additional setup
    //--------------------------------------------------------------------------------------
    DefaultQualities test_qualities = DefaultQualities();
    test_qualities.SetQuality(QualityTypes::Mass, 10.0);
    test_qualities.SetQuality(QualityTypes::Charge, 5.0);
    
    ParticleType test_particles = ParticleType(5.0, test_qualities, Color{240, 50, 20, 255});
    //for (int i = 0; i < 3; i++) {
    //    for (int j = 0; j < 3; j++) {
    //        Vector2 new_position = Vector2 {(float)50*i + positionOffset.x, (float)50*j + positionOffset.y};
    //        Vector2 new_velocity = Vector2 {(float)(i-1)*j, (float)(j-1)*i};
    //        test_particles.addParticle(new_position, new_velocity);
    //    }
    //}

    test_particles.addParticle(Vector2{300,300}, Vector2{2,0});
    test_particles.addParticle(Vector2{700,300}, Vector2{0,0});
    test_particles.addParticle(Vector2{730,300}, Vector2{0,0});
    test_particles.addParticle(Vector2{760,300}, Vector2{0,0});

    test_particles.addParticle(Vector2{820,315}, Vector2{0,0});
    test_particles.addParticle(Vector2{820,285}, Vector2{0,0});


    int temp_num_particles = 6;
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

        if (IsKeyPressed(KEY_E) || IsKeyDown(KEY_R)) { // E for 1 until next press. R for 1 per frame while held.
            Vector2 position = GetMousePosition();
            test_particles.addParticle(position, Vector2{0.0,0.0});
            temp_num_particles += 1;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            int x = textPositionOffset.x;
            int y = textPositionOffset.y;
            //DrawText("move the ball with arrow keys\nmove the text with wasd", 10+x, 10+y, 20, DARKGRAY);
            DrawFPS(10,10);
            const char *text = TextFormat("Number of Particles: %d", temp_num_particles);
            DrawText(text, 10,40, 20, DARKGRAY);

            // For every test particle.
            for (int i = 0; i<temp_num_particles; i++) {
                Vector2 pos = test_particles.getPosition(i);            // Get the position of the particle.
                DrawCircleV(pos, test_particles.getRadius(), test_particles.getDefaultColor());   // Draw a circle at the particle's position.

                pos = Vector2Add(pos, test_particles.getVelocity(i));   // Get the particle's velocity.
                test_particles.setPosition(i, pos);                     // Use the velocity to update the particle's position.
            }
            
            int max_loops = 20;
            bool collision_detected = true;
            outerloop: while (collision_detected && max_loops > 0) {
                max_loops--;
                collision_detected = false;
                
                for (int i=0; i<temp_num_particles; i++) {
                    Vector2 pos = test_particles.getPosition(i);
                    Vector2 vel = test_particles.getVelocity(i);
                    int radius = test_particles.getRadius();
                    if (pos.x > screenWidth-radius && vel.x > 0) {
                        vel.x *= -0.5;
                        test_particles.setVelocity(i, vel);
                        collision_detected = true;
                    } else if (pos.x < radius && vel.x < 0) {
                        vel.x *= -0.5;
                        test_particles.setVelocity(i, vel);
                        collision_detected = true;
                    }
                    if (pos.y > screenHeight-radius && vel.y > 0) {
                        vel.y *= -0.5;
                        test_particles.setVelocity(i, vel);
                        collision_detected = true;
                    } else if (pos.y < radius && vel.y < 0) {
                        vel.y *= -0.5;
                        test_particles.setVelocity(i, vel);
                        collision_detected = true;
                    }

                    for (int j=i+1; j<temp_num_particles; j++) {
                        Vector2 diff = Vector2Subtract(test_particles.getPosition(i), test_particles.getPosition(j));
                        if (Vector2Length(diff) >= test_particles.getRadius()*2) {
                            continue;
                        }
                        
                        // Motion is relative, so we treat the collision as particle i having all the motion and particle j being stationary.
                        Vector2 v_diff = Vector2Subtract(test_particles.getVelocity(i), test_particles.getVelocity(j));
                        
                        Vector2 p_diff_normalized = Vector2Normalize(diff);
                        
                        float dot_prod = Vector2DotProduct(p_diff_normalized, v_diff);
                        if (dot_prod > 0) { // If particles are already moving away from each other, we just let them.
                            continue;
                        }
                        collision_detected = true;
                        
                        Vector2 impact_velocity = Vector2Scale(p_diff_normalized, dot_prod);
                        
                        Vector2 vi1 = impact_velocity;
                        Vector2 vj1 = Vector2{0,0};
                        
                        float mA = test_particles.getQuality(Mass);
                        float mB = test_particles.getQuality(Mass);
                        
                        Vector2 vi2 = Vector2Add( Vector2Scale(vi1, (mA-mB)/(mA+mB)), Vector2Scale(vj1, (2*mB)/(mA+mB)) );
                        Vector2 vj2 = Vector2Add( Vector2Scale(vj1, (mB-mA)/(mA+mB)), Vector2Scale(vi1, (2*mA)/(mA+mB)) );
                        
                        test_particles.addMomentumChange(i, Vector2Negate(Vector2Add(impact_velocity, vi2)));
                        test_particles.addMomentumChange(j, vj2);
                        
                        
                    }
                }

                if (!collision_detected) {
                    break;
                }
                test_particles.applyMomentumChanges();
                for (int i=0; i<temp_num_particles; i++) {
                    for (int j=0; j<temp_num_particles; j++) {
                        test_particles.setPosition(i, test_particles.getPriorPosition(i)+test_particles.getVelocity(i));
                        test_particles.setPosition(j, test_particles.getPriorPosition(j)+test_particles.getVelocity(j));
                    }
                }
            }
            
            test_particles.readyForNextPosition();
                        
            for (int i = 0; i<temp_num_particles; i++) {
                for (int j=i+1; j<temp_num_particles; j++) {
                    Vector2 pos1 = test_particles.getPosition(i);
                    Vector2 pos2 = test_particles.getPosition(j);
                    float m = test_particles.getQuality(QualityTypes::Mass);

                    Vector2 diff = Vector2Subtract(pos2, pos1);
                    float diff_magnitude_squared = Vector2LengthSqr(diff);
                    if (diff_magnitude_squared < test_particles.getRadius()*test_particles.getRadius()) {
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