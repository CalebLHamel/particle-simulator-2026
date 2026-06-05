#include <iostream>
#include "raylib.h"
#include "particle.h"
#include "raymath.h"
#include "particle_type.h"
#include "particle.h"
#include <vector>

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
    Qualities test_qualities = Qualities();
    test_qualities.setQuality(QualityTypes::Mass, 10.0);
    test_qualities.setQuality(QualityTypes::Charge, 10.0);
    
    ParticleType test_particle_type = ParticleType(10.0, test_qualities, Color{240, 50, 20, 255});
    Qualities test_qualities2 = Qualities();
    test_qualities2.setQuality(QualityTypes::Mass, 10.0);
    test_qualities2.setQuality(QualityTypes::Charge, 0.0);
    ParticleType test_particle_type2 = ParticleType(10.0, test_qualities2, Color{240, 240, 20, 255});

    Qualities test_qualities0 = Qualities();
    test_qualities0.setQuality(QualityTypes::Mass, 10.0);
    test_qualities0.setQuality(QualityTypes::Charge, -10.0);
    ParticleType test_particle_type0 = ParticleType(10.0, test_qualities0, Color{20, 20, 240, 255});

    ParticleType types[3] = {test_particle_type0, test_particle_type, test_particle_type2};
    int type_selection = 0;

    std::vector<Particle> particles = std::vector<Particle>();

    //particles.push_back(Particle::makeParticleFromType(test_particle_type, Vector2{270,300}, Vector2{0,0}));
    //particles.push_back(Particle::makeParticleFromType(test_particle_type2, Vector2{300,300}, Vector2{0,0}));
    //particles.push_back(Particle::makeParticleFromType(test_particle_type, Vector2{700,300}, Vector2{0,0}));
    //particles.push_back(Particle::makeParticleFromType(test_particle_type0, Vector2{730,300}, Vector2{0,0}));
    //particles.push_back(Particle::makeParticleFromType(test_particle_type0, Vector2{760,300}, Vector2{0,0}));
    //particles.push_back(Particle::makeParticleFromType(test_particle_type, Vector2{820,315}, Vector2{0,0}));
    //particles.push_back(Particle::makeParticleFromType(test_particle_type, Vector2{820,285}, Vector2{0,0}));


    int count60 = 0;
    float total_kinetic_energy = 0;
    Vector2 launch_velocity = Vector2{0,0};

    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_LEFT))    launch_velocity.x -= 1.0f;
        if (IsKeyPressed(KEY_RIGHT))   launch_velocity.x += 1.0f;
        if (IsKeyPressed(KEY_UP))      launch_velocity.y -= 1.0f;
        if (IsKeyPressed(KEY_DOWN))    launch_velocity.y += 1.0f;

        if (IsKeyPressed(KEY_D)) textPositionOffset.x += 4.0f;
        if (IsKeyPressed(KEY_A)) textPositionOffset.x -= 4.0f;
        if (IsKeyPressed(KEY_W)) textPositionOffset.y -= 4.0f;
        if (IsKeyPressed(KEY_S)) textPositionOffset.y += 4.0f;

        if (IsKeyPressed(KEY_E) || IsKeyDown(KEY_R)) { // E for 1 until next press. R for 1 per frame while held.
            Vector2 position = GetMousePosition();
            particles.push_back(Particle::makeParticleFromType(types[type_selection], position, launch_velocity));
        }

        if (IsKeyPressed(KEY_ONE)) {
            type_selection = 0;
        } else if (IsKeyPressed(KEY_TWO)) {
            type_selection = 1;
        } else if (IsKeyPressed(KEY_THREE)) {
            type_selection = 2;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            int x = textPositionOffset.x;
            int y = textPositionOffset.y;
            //DrawText("move the ball with arrow keys\nmove the text with wasd", 10+x, 10+y, 20, DARKGRAY);
            DrawFPS(10,10);
            const char *text = TextFormat("Number of Particles: %d", particles.size());
            DrawText(text, 10,40, 20, DARKGRAY);

            count60 += 1;
            if (count60 == 60) {
                count60 = 0;
                total_kinetic_energy = 0;
                for (int i=0; i<particles.size(); i++) {
                    float v = Vector2Length(particles[i].getVelocity());
                    total_kinetic_energy += 0.5*v*v*particles[i].getQuality(Mass);
                }
            }
            const char *text2 = TextFormat("Total Kinetic Energy: %d", (int)total_kinetic_energy);
            DrawText(text2, 10, 70, 20, DARKGRAY);

            const char *text3 = TextFormat("Launch Velocity: (%d, %d)", (int)launch_velocity.x, (int)launch_velocity.y);
            DrawText(text3, 10, 100, 20, DARKGRAY);

            // For every test particle.
            for (int i = 0; i<particles.size(); i++) {
                Vector2 pos = particles[i].getPosition();            // Get the position of the particle.
                DrawCircleV(pos, particles[i].getRadius(), particles[i].getColor());   // Draw a circle at the particle's position.
                
                pos = Vector2Add(pos, particles[i].getVelocity());   // Get the particle's velocity.
                particles[i].setPosition(pos);                     // Use the velocity to update the particle's position.
            }
        EndDrawing();
            
            int max_loops = 20;
            bool collision_detected = true;

            // todo: Replace the indexing with variables to improve readability.
            outerloop: while (collision_detected && max_loops > 0) {
                max_loops--;
                collision_detected = false;
                
                for (int i=0; i<particles.size(); i++) {
                    float wall_dampening = 0.9;
                    Vector2 pos = particles[i].getPosition();
                    Vector2 vel = particles[i].getVelocity();
                    int radius = particles[i].getRadius();
                    if (pos.x > screenWidth-radius && vel.x > 0) {
                        vel.x *= -wall_dampening;
                        particles[i].setVelocity(vel);
                        collision_detected = true;
                    } else if (pos.x < radius && vel.x < 0) {
                        vel.x *= -wall_dampening;
                        particles[i].setVelocity(vel);
                        collision_detected = true;
                    }
                    if (pos.y > screenHeight-radius && vel.y > 0) {
                        vel.y *= -wall_dampening;
                        particles[i].setVelocity(vel);
                        collision_detected = true;
                    } else if (pos.y < radius && vel.y < 0) {
                        vel.y *= -wall_dampening;
                        particles[i].setVelocity(vel);
                        collision_detected = true;
                    }

                    for (int j=i+1; j<particles.size(); j++) {
                        Vector2 diff = Vector2Subtract(particles[i].getPosition(), particles[j].getPosition());
                        if (Vector2Length(diff) >= particles[i].getRadius()+particles[j].getRadius()) {
                            continue;
                        }
                        
                        // Motion is relative, so we treat the collision as particle i having all the motion and particle j being stationary.
                        Vector2 v_diff = Vector2Subtract(particles[i].getVelocity(), particles[j].getVelocity());
                        
                        Vector2 p_diff_normalized = Vector2Normalize(diff);
                        
                        float dot_prod = Vector2DotProduct(p_diff_normalized, v_diff);
                        if (dot_prod > 0) { // If particles are already moving away from each other, we just let them.
                            continue;
                        }
                        collision_detected = true;
                        
                        Vector2 impact_velocity = Vector2Scale(p_diff_normalized, dot_prod);
                        
                        Vector2 residual_velocity = Vector2Subtract(v_diff, impact_velocity);
                        residual_velocity = Vector2Add(residual_velocity, particles[j].getVelocity());

                        Vector2 vi1 = impact_velocity;
                        Vector2 vj1 = Vector2{0,0};
                        
                        float mA = particles[i].getQuality(Mass);
                        float mB = particles[j].getQuality(Mass);
                        
                        Vector2 vi2 = Vector2Add( Vector2Scale(vi1, (mA-mB)/(mA+mB)), Vector2Scale(vj1, (2*mB)/(mA+mB)) );
                        Vector2 vj2 = Vector2Add( Vector2Scale(vj1, (mB-mA)/(mA+mB)), Vector2Scale(vi1, (2*mA)/(mA+mB)) );
                        
                        particles[i].setVelocity(Vector2Add(particles[i].getVelocity(), Vector2Subtract(vi2, vi1)));
                        particles[j].setVelocity(Vector2Add(particles[j].getVelocity(), Vector2Subtract(vj2, vj1)));

                        particles[i].setPosition(particles[i].getPriorPosition()+particles[i].getVelocity());
                        particles[j].setPosition(particles[j].getPriorPosition()+particles[j].getVelocity());
                    }
                }
            }
            
            for (int i=0; i<particles.size(); i++) {
                particles[i].readyForNewPosition();
            }
            
            for (int i = 0; i<particles.size(); i++) {
                for (int j=i+1; j<particles.size(); j++) {
                    Vector2 pos1 = particles[i].getPosition();
                    Vector2 pos2 = particles[j].getPosition();
                    
                    float m1 = particles[i].getQuality(Mass);
                    float m2 = particles[j].getQuality(Mass);
                    
                    float q1 = particles[i].getQuality(Charge);
                    float q2 = particles[j].getQuality(Charge);

                    Vector2 diff = Vector2Subtract(pos2, pos1);
                    float diff_magnitude_squared = Vector2LengthSqr(diff);
                    
                    //float acc_magnitude = (m1*m2) / diff_magnitude_squared;
                    float acc_magnitude = -(q1*q2) / diff_magnitude_squared;

                    Vector2 acc1 = Vector2Scale(Vector2Normalize(diff), 3.0*acc_magnitude / m1);
                    Vector2 acc2 = Vector2Scale(Vector2Normalize(diff), 3.0*acc_magnitude / m2);

                    Vector2 vel1 = particles[i].getVelocity();
                    Vector2 vel2 = particles[j].getVelocity();
                    
                    vel1 = Vector2Add(vel1, acc1);
                    vel2 = Vector2Subtract(vel2, acc2);
                    particles[i].setVelocity(vel1);
                    particles[j].setVelocity(vel2);
                }
            }

        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}