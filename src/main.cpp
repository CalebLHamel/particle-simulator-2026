#include <iostream>
#include "raylib.h"
#include "particle.hpp"
#include "raymath.h"
#include "particle_type.hpp"
#include "particle.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include "simulation.hpp"
#include "rlgl.h"

int main()
{
    /*
     * This is an extensive modification of a basic example from Raylib's website.
     * Hardly any of the original remains by this point.
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
    test_qualities0.setQuality(QualityTypes::Mass, 3.0);
    test_qualities0.setQuality(QualityTypes::Charge, -4.0);
    ParticleType test_particle_type0 = ParticleType(4.0, test_qualities0, Color{20, 20, 240, 255});
    
    Qualities test_qualities1 = Qualities();
    test_qualities1.setQuality(QualityTypes::Mass, 30.0);
    test_qualities1.setQuality(QualityTypes::Charge, 120.0);
    ParticleType test_particle_type1 = ParticleType(10.0, test_qualities1, Color{240, 50, 20, 255});
    
    Qualities test_qualities2 = Qualities();
    test_qualities2.setQuality(QualityTypes::Mass, 30.0);
    test_qualities2.setQuality(QualityTypes::Charge, 0.0);
    ParticleType test_particle_type2 = ParticleType(10.0, test_qualities2, Color{240, 240, 20, 255});

    Qualities test_qualities3 = Qualities();
    test_qualities3.setQuality(QualityTypes::Mass, 30.0);
    test_qualities3.setQuality(QualityTypes::Charge, -120.0);
    ParticleType test_particle_type3 = ParticleType(10.0, test_qualities3, Color{40, 40, 60, 255});

    ParticleType types[4] = {test_particle_type0, test_particle_type1, test_particle_type2, test_particle_type3};
    
    // The simulation itself.
    size_t chunks_wide = 30;
    size_t chunks_tall = 30;
    float chunk_size = 600;
    Simulation simulation = Simulation(chunks_wide, chunks_tall, chunk_size, 1500, 10);
    
    // Main loop
    //--------------------------------------------------------------------------------------
    
    // Some variable for use in the loop.
    int type_selection = 0;
    Vector2 launch_velocity = Vector2{0,0};
    Vector2 camera_offset = Vector2{screenWidth/2, screenHeight/2};
    Vector2 camera_position = Vector2{0,0};
    float camera_scale = 1.0;
    
    // Creating a basic circle texture.
    int textureSize = 128;
    Image circle_image = GenImageColor(textureSize, textureSize, BLANK);
    ImageDrawCircle(&circle_image, textureSize/2, textureSize/2, textureSize/2, WHITE);
    Texture2D circle_texture = LoadTextureFromImage(circle_image);
    UnloadImage(circle_image); // Unloading CPU-side image.
    
    // Used for the rendering loop.
    std::vector<Vector2> positions = std::vector<Vector2>(100000);
    std::vector<float> radii = std::vector<float>(100000);
    std::vector<Color> colors = std::vector<Color>(100000);

    double force_time = 0;
    double collision_time = 0;
    double move_time = 0;
    
    while (!WindowShouldClose())    // Detect window close button or ESC keyc
    {
        // Arrow keys affect initial velocity of new particles.
        if (IsKeyPressed(KEY_LEFT))    launch_velocity.x -= 1.0f;
        if (IsKeyPressed(KEY_RIGHT))   launch_velocity.x += 1.0f;
        if (IsKeyPressed(KEY_UP))      launch_velocity.y -= 1.0f;
        if (IsKeyPressed(KEY_DOWN))    launch_velocity.y += 1.0f;

        // WASD move the camera around.
        if (IsKeyDown(KEY_A))    camera_position.x -= 10.0f * camera_scale;
        if (IsKeyDown(KEY_D))    camera_position.x += 10.0f * camera_scale;
        if (IsKeyDown(KEY_W))    camera_position.y -= 10.0f * camera_scale;
        if (IsKeyDown(KEY_S))    camera_position.y += 10.0f * camera_scale;

        // E for 1 particle until next press. R for 1 per frame while held.
        if (IsKeyPressed(KEY_E) || IsKeyDown(KEY_R)) {
            Vector2 position = GetMousePosition();
            position = Vector2{(position.x-camera_offset.x)*camera_scale+camera_offset.x+camera_position.x,(position.y-camera_offset.y)*camera_scale+camera_offset.y+camera_position.y};

            Particle new_particle = Particle::makeParticleFromType(types[type_selection], position, launch_velocity);
            simulation.addParticle(new_particle);
            //particles.push_back(Particle::makeParticleFromType(types[type_selection], position, launch_velocity));
        }
        // Spawns many particles at a time, per frame, while T is held.
        if (IsKeyDown(KEY_T)) {
            Vector2 position = GetMousePosition();
            Vector2 base_position = Vector2{(position.x-camera_offset.x)*camera_scale+camera_offset.x+camera_position.x,(position.y-camera_offset.y)*camera_scale+camera_offset.y+camera_position.y};

            float offset = types[type_selection].getRadius()*10;
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    Vector2 newpos = Vector2Add(base_position, Vector2{i*offset, j*offset});
                    Particle new_particle = Particle::makeParticleFromType(types[type_selection], newpos, launch_velocity);
                    simulation.addParticle(new_particle);
                }
            } 
        }

        // Used during some demonstrations. Just fills the simulation space with a random assortment of particles.
        if (IsKeyPressed(KEY_P)) {
            for (int i=0; i<500; i+=1) {
                float x = GetRandomValue(0, chunks_wide*chunk_size);
                float y = GetRandomValue(0, chunks_tall*chunk_size);
                Vector2 pos = {x,y};
                Particle new_particle = Particle::makeParticleFromType(types[GetRandomValue(1,3)], pos, {GetRandomValue(-1,1),GetRandomValue(-1,1)});
                simulation.addParticle(new_particle);
            }
        }

        // Z zooms in, X zooms out.
        if (IsKeyDown(KEY_Z)) camera_scale = std::max(0.1f,camera_scale - 0.1f);
        if (IsKeyDown(KEY_X)) camera_scale = camera_scale + 0.1f;

        // Number row used to select types.
        if (IsKeyPressed(KEY_ONE)) {
            type_selection = 0;
        } else if (IsKeyPressed(KEY_TWO)) {
            type_selection = 1;
        } else if (IsKeyPressed(KEY_THREE)) {
            type_selection = 2;
        } else if (IsKeyPressed(KEY_FOUR)) {
            type_selection = 3;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            // Grey background/
            ClearBackground(Color{100,100,100,255});

            // White simualtion area.
            DrawRectangle((-camera_offset.x-camera_position.x)/camera_scale+camera_offset.x,(-camera_offset.y-camera_position.y)/camera_scale+camera_offset.y,chunks_wide*chunk_size/camera_scale, chunks_tall*chunk_size/camera_scale, RAYWHITE);
            
            // The bound of where the camera can see particles.
            float camera_left_bound = std::max(0.0f,(camera_position.x + camera_offset.x) - (screenWidth/2) * camera_scale);
            float camera_right_bound = std::max(0.0f,(camera_position.x + camera_offset.x) + (screenWidth/2) * camera_scale);
            float camera_upper_bound = std::max(0.0f,(camera_position.y + camera_offset.y) - (screenHeight/2) * camera_scale);
            float camera_lower_bound = std::max(0.0f,(camera_position.y + camera_offset.y) + (screenHeight/2) * camera_scale);

            // The bounds of what chunks are visible.
            size_t xo = floor(camera_left_bound / chunk_size);
            size_t yo = floor(camera_upper_bound / chunk_size);
            size_t xf = std::min(chunks_wide, (size_t)ceil(camera_right_bound / chunk_size));
            size_t yf = std::min(chunks_tall, (size_t)ceil(camera_lower_bound / chunk_size));

            // For each chunk.
            size_t j = 0;
            for (size_t x = xo; x < xf; x += 1) {
                for (size_t y = yo; y < yf; y += 1) {
                    // For each particle in the chunk.
                    std::vector<Particle>* particles_in_chunk = simulation.getParticlesInChunk(x,y);
                    
                    // Draw the particle.
                    for (size_t i = 0; i < particles_in_chunk->size(); i += 1) {
                        Particle* particle = &(*particles_in_chunk)[i];
                        Vector2 position = Vector2Scale(Vector2Subtract(Vector2Subtract(particle->getPosition(), camera_position), camera_offset), 1/camera_scale);
                        position = Vector2Add(position, camera_offset);
                        positions[j] = position;
                        radii[j] = particle->getRadius() / camera_scale;
                        colors[j] = particle->getColor();
                        j += 1;
                    }
                }
            }

            // Draw a circle texture, appropiately scaled and coloured, for every particle that is visible.
            Rectangle source = {0,0,(float)circle_texture.width, (float)circle_texture.height};
            for (size_t k=0; k<j; k+=1) {
                float r = radii[k];
                Rectangle destination = {positions[k].x-r, positions[k].y-r, r*2, r*2};
                DrawTexturePro(circle_texture, source, destination, {0,0}, 0.0f, colors[k]);
            }

            // Background for some display information.
            DrawRectangle(5,5,375, 215, Color{30,30,30,200});
            
            DrawFPS(10,10);
            
            const char *text = TextFormat("Number of Particles: %d", simulation.getCount());
            DrawText(text, 10,40, 20, WHITE);

            const char *text2 = TextFormat("Camera: (%d, %d). Zoom: %f", (int)camera_position.x, (int)camera_position.y, (float)(1/camera_scale));
            DrawText(text2, 10,70, 20, WHITE);

            const char *text3 = TextFormat("Launch Velocity: (%d, %d)", (int)launch_velocity.x, (int)launch_velocity.y);
            DrawText(text3, 10, 100, 20, WHITE);

            const char *text4 = TextFormat("MT: %dms\nFT: %dms\nCT: %dms\n", (int)move_time, (int)force_time, (int)collision_time);
            DrawText(text4, 10,130,20,WHITE);

        EndDrawing();

        double time = 0;
        time = (GetTime()*1000);
        simulation.moveParticles();
        time = (GetTime()*1000) - time;
        move_time = (59*move_time + time) / 60;

        time = (GetTime()*1000);
        simulation.manageCollisions();
        time = (GetTime()*1000) - time;
        collision_time = (59*collision_time + time) / 60;

        time = (GetTime()*1000);
        simulation.determineForces();
        time = (GetTime()*1000) - time;
        force_time = (59*force_time + time) / 60;
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}