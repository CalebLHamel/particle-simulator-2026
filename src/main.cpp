#include <iostream>
#include "raylib.h"
#include "particle.h"
#include "raymath.h"
#include "particle_type.h"
#include "particle.h"
#include <vector>
#include <cmath>
#include "simulation.h"

// TODO: Modularize the simulation. I'll likely do so as I optimize it along the way and continue cleaning up.

int main()
{
    /*
     * This is an extensive modification of a basic example from Raylib's website.
     */

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 1000;
    
    InitWindow(screenWidth, screenHeight, "Learning Things");
    
    // Set our game to run at 60 frames-per-second
    SetTargetFPS(60);
    
    // Simulation setup
    //--------------------------------------------------------------------------------------
    
    // Defining a few different particle types to use.
    
    Qualities test_qualities0 = Qualities();
    test_qualities0.setQuality(QualityTypes::Mass, 1.0);
    test_qualities0.setQuality(QualityTypes::Charge, -4.0);
    ParticleType test_particle_type0 = ParticleType(4.0, test_qualities0, Color{20, 20, 240, 255});
    
    Qualities test_qualities1 = Qualities();
    test_qualities1.setQuality(QualityTypes::Mass, 10.0);
    test_qualities1.setQuality(QualityTypes::Charge, 120.0);
    ParticleType test_particle_type1 = ParticleType(10.0, test_qualities1, Color{240, 50, 20, 255});
    
    Qualities test_qualities2 = Qualities();
    test_qualities2.setQuality(QualityTypes::Mass, 10.0);
    test_qualities2.setQuality(QualityTypes::Charge, 0.0);
    ParticleType test_particle_type2 = ParticleType(10.0, test_qualities2, Color{240, 240, 20, 255});
    ParticleType types[3] = {test_particle_type0, test_particle_type1, test_particle_type2};
    
    // The simulation itself.
    size_t chunks_wide = 32;
    size_t chunks_tall = 20;
    Simulation simulation = Simulation(chunks_wide, chunks_tall, 50, 1000, 5);
    
    // Main loop
    //--------------------------------------------------------------------------------------
    
    // Some variable for use in the loop.
    int type_selection = 0;
    Vector2 launch_velocity = Vector2{0,0};

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Arrow keys affect initial velocity of new particles.
        if (IsKeyPressed(KEY_LEFT))    launch_velocity.x -= 1.0f;
        if (IsKeyPressed(KEY_RIGHT))   launch_velocity.x += 1.0f;
        if (IsKeyPressed(KEY_UP))      launch_velocity.y -= 1.0f;
        if (IsKeyPressed(KEY_DOWN))    launch_velocity.y += 1.0f;

        // E for 1 particle until next press. R for 1 per frame while held.
        if (IsKeyPressed(KEY_E) || IsKeyDown(KEY_R)) {
            Vector2 position = GetMousePosition();
            Particle new_particle = Particle::makeParticleFromType(types[type_selection], position, launch_velocity);
            simulation.addParticle(new_particle);
            //particles.push_back(Particle::makeParticleFromType(types[type_selection], position, launch_velocity));
        }
        // Spawns 9 particles at a time, per frame, while T is held.
        if (IsKeyDown(KEY_T)) {
            Vector2 base_position = GetMousePosition();
            float offset = types[type_selection].getRadius()*10;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    Vector2 newpos = Vector2Add(base_position, Vector2{i*offset, j*offset});
                    Particle new_particle = Particle::makeParticleFromType(types[type_selection], newpos, launch_velocity);
                    simulation.addParticle(new_particle);
                }
            } 
        }

        // Number row used to select types.
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
            
            // For each chunk.
            for (size_t x = 0; x < chunks_wide; x += 1) {
                for (size_t y = 0; y < chunks_tall; y += 1) {
                    // For each particle in the chunk.
                    std::vector<Particle>* particles_in_chunk = simulation.getParticlesInChunk(x,y);
                    
                    // Draw the particle.
                    for (size_t i = 0; i < particles_in_chunk->size(); i += 1) {
                        Particle* particle = &(*particles_in_chunk)[i];
                        DrawCircleV(particle->getPosition(), particle->getRadius(), particle->getColor());
                    }
                }
            }
            
            // Background for some display information.
            DrawRectangle(5,5,275, 125, Color{30,30,30,200});
            
            DrawFPS(10,10);
            
            const char *text = TextFormat("Number of Particles: %d", simulation.getCount());
            DrawText(text, 10,40, 20, WHITE);
            
            const char *text3 = TextFormat("Launch Velocity: (%d, %d)", (int)launch_velocity.x, (int)launch_velocity.y);
            DrawText(text3, 10, 100, 20, WHITE);
        
        EndDrawing();

        simulation.moveParticles();
        simulation.manageCollisions();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}