#include <iostream>
#include "raylib.h"
#include "particle.h"
#include "raymath.h"
#include "particle_type.h"
#include "particle.h"
#include <vector>
#include <cmath>

// TODO: Modularize the simulation. I'll likely do so as I optimize it along the way and continue cleaning up.

int main()
{
    /*
     * This is an extensive modification of a basic example from Raylib's website.
     */

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 900;

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

    // Holds all the particles in the simulation.
    std::vector<Particle> particles = std::vector<Particle>();
    
    // Main game loop
    //--------------------------------------------------------------------------------------

    // Some variable for use in the loop.
    ParticleType types[3] = {test_particle_type0, test_particle_type1, test_particle_type2};
    int type_selection = 0;
    int count60 = 0;
    float total_kinetic_energy = 0;
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
            particles.push_back(Particle::makeParticleFromType(types[type_selection], position, launch_velocity));
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

            // Some display information.
            DrawFPS(10,10);
            const char *text = TextFormat("Number of Particles: %d", particles.size());
            DrawText(text, 10,40, 20, DARKGRAY);

            // Every 60 frames, get the total kinetic energy of the simulation.
            count60 += 1;
            if (count60 == 60) {
                count60 = 0;
                total_kinetic_energy = 0;
                for (int i=0; i<particles.size(); i++) {
                    float v = Vector2Length(particles[i].getVelocity());
                    total_kinetic_energy += 0.5*v*v*particles[i].getQuality(Mass);
                }
            }
            // Print the total kinetic energy of the simulation, rounded down to the nearest integer.
            const char *text2 = TextFormat("Total Kinetic Energy: %d", (int)total_kinetic_energy);
            DrawText(text2, 10, 70, 20, DARKGRAY);

            // Print the current launch velocity for new particles.
            const char *text3 = TextFormat("Launch Velocity: (%d, %d)", (int)launch_velocity.x, (int)launch_velocity.y);
            DrawText(text3, 10, 100, 20, DARKGRAY);

            // For every test particle.
            for (int i = 0; i<particles.size(); i++) {
                Vector2 pos = particles[i].getPosition();            
                
                // Draw a circle at the particle's position.
                DrawCircleV(pos, particles[i].getRadius(), particles[i].getColor());
                
                // Get the particle's position, and add its velocity to it to get the new position.
                Vector2 new_pos = Vector2Add(pos, particles[i].getVelocity());
                // Update the particle's position.
                particles[i].setPosition(new_pos);                     
            }
        EndDrawing();
            
        // Break out of the loop if its taking too many iteration to solve.
        // In such a scenario, I decided to let the simulation try again next frame
        // to avoid excessive freezes.
        int max_loops = 20;

        // A helper variable. We run the loop until there are no more collisions to resolve.
        bool collision_detected = true;

        // TODO: Replace the indexing with variables to improve readability.
        // While there may be collisions to resolve, and the max loops haven't been exceeded.
        while (collision_detected && max_loops > 0) {
            // Decrement max loops counter.
            max_loops--;
            // Now that we're in the loop, assume there are no collisions until it's potentially found otherwise.
            collision_detected = false;
            
            // For every particle.
            for (int i=0; i<particles.size(); i++) {
                // Bounce it off of the wall if it has to.
                // Wall dampening can be used to reduce a particle's speed on striking a wall.
                float wall_dampening = 0.9;

                Vector2 pos = particles[i].getPosition();
                Vector2 vel = particles[i].getVelocity();
                int radius = particles[i].getRadius();
                // If beyond x limit, and still moving away.
                // This logic is used for all these conditionals in this block.
                if (pos.x > screenWidth-radius && vel.x > 0) {
                    // Reverse the velocity.
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

                // For every particle that it may bounce off of, that it hasn't been compared to yet (each subsequent particle in the list, so we test all combinations).
                for (int j=i+1; j<particles.size(); j++) {
                    
                    // Particle i's position relative to particle j.
                    Vector2 position_difference = Vector2Subtract(particles[i].getPosition(), particles[j].getPosition());
                    // If the particles farther apart than the sum of their radii, they are not colliding.
                    if (Vector2Length(position_difference) >= particles[i].getRadius()+particles[j].getRadius()) {
                        // Check the next particle in the list.
                        continue;
                    }
                    
                    // Motion is relative, so we treat the collision as particle i having all the motion and particle j being stationary.
                    Vector2 velocity_i_relative_to_j = Vector2Subtract(particles[i].getVelocity(), particles[j].getVelocity());
                    
                    // Get the direction from j to i.
                    Vector2 position_difference_direction = Vector2Normalize(position_difference);
                    
                    // The dot product of the position difference and the relative velocity tells us how direct of an impact it is.
                    float impact_speed = Vector2DotProduct(position_difference_direction, velocity_i_relative_to_j);
                    
                    // If particles are already moving away from each other, we just let them continue on and ignore the collision.
                    if (impact_speed > 0) { 
                        continue;
                    }
                    // Else, we detected a collision to resolve, which mean changes happen, and another iteration will be necessary.
                    collision_detected = true;
                    
                    // We scale the direction in the position difference (the direction of the impact) by the speed of the impact, to get the impact velocity.
                    Vector2 impact_velocity = Vector2Scale(position_difference_direction, impact_speed);
                    
                    // As we are working in the reference frame of particle j being stationary,
                    // particle i has all the velocity of the impact.
                    // This is used to simplify some momentum calculations.
                    
                    float mass_i = particles[i].getQuality(Mass);
                    float mass_j = particles[j].getQuality(Mass);
                    
                    Vector2 change_in_velocity_i = Vector2Subtract(Vector2Scale(impact_velocity, (mass_i-mass_j)/(mass_i+mass_j)), impact_velocity);
                    Vector2 change_in_velocity_j = Vector2Scale(impact_velocity, (2*mass_i)/(mass_i+mass_j));
                    
                    // The particles' final velocities are their initial velocities, plus their change in velocity.
                    particles[i].setVelocity(Vector2Add(particles[i].getVelocity(), change_in_velocity_i));
                    particles[j].setVelocity(Vector2Add(particles[j].getVelocity(), change_in_velocity_j));

                    // Update where the particles would end up based on the velocity changes from the collision.
                    // This is why the main loop has to interate until no collisions cause a change, because a change
                    // in the positions as the result of collisions may incur more collisions.
                    particles[i].setPosition(particles[i].getPriorPosition()+particles[i].getVelocity());
                    particles[j].setPosition(particles[j].getPriorPosition()+particles[j].getVelocity());
                }
            }
        }
        
        // Set every particle's prior position to their current position.
        for (int i=0; i<particles.size(); i++) {
            particles[i].readyForNewPosition();
        }
        
        // Calculate force effects on every particle.
        for (int i = 0; i<particles.size(); i++) {
            // To check every combination, each particle checks itself against the subsequent particles in the list.
            for (int j=i+1; j<particles.size(); j++) {
                Vector2 position_i = particles[i].getPosition();
                Vector2 position_j = particles[j].getPosition();
                
                float mass_i = particles[i].getQuality(Mass);
                float mass_j = particles[j].getQuality(Mass);
                
                float charge_i = particles[i].getQuality(Charge);
                float charge_j = particles[j].getQuality(Charge);

                // Get where particle j is relative to particle i.
                Vector2 relative_position_of_j = Vector2Subtract(position_j, position_i);

                // Get the magnitude of the distance squared.
                float diff_magnitude_squared = Vector2LengthSqr(relative_position_of_j);
                
                // Gravity effects.
                //float force_magnitude = (m1*m2) / diff_magnitude_squared;
                
                // Charge effects.
                //float force_magnitude = -(charge_i*charge_j) / diff_magnitude_squared;

                // Funny charge effect.
                float x = Vector2Length(relative_position_of_j);
                float funny_magnitude = (x-40)*(pow(2.71, -((x-40)*(x-40))/2000))/10000;
                float funny_i = 0;
                float funny_j = 0;
                if ((charge_i > 0 && charge_j < 0) || (charge_i < 0 && charge_j > 0)) {
                    funny_i = funny_magnitude * Vector2DotProduct(Vector2Subtract(particles[j].getVelocity(), particles[i].getVelocity()), relative_position_of_j);
                    funny_j = funny_magnitude * Vector2DotProduct(Vector2Subtract(particles[j].getVelocity(), particles[i].getVelocity()), relative_position_of_j);
                }
                float force_magnitude = -(charge_i*charge_j) * funny_magnitude;


                // force / mass is the acceleration, which then scales the direction between the particles to get acceleration vectors.
                Vector2 acceleration_i = Vector2Scale(Vector2Normalize(relative_position_of_j), funny_i / mass_i+ 3.0*force_magnitude / mass_i);
                Vector2 acceleration_j = Vector2Scale(Vector2Normalize(relative_position_of_j), funny_j / mass_j + 3.0*force_magnitude / mass_j);

                // Get velocities.
                Vector2 velocity_i = particles[i].getVelocity();
                Vector2 velocity_j = particles[j].getVelocity();
                
                // Accelerate them by adding (or subtracting, for j because of orientations) the accelerations.
                velocity_i = Vector2Add(velocity_i, acceleration_i);
                velocity_j = Vector2Subtract(velocity_j, acceleration_j);

                // Set the new velocities.
                particles[i].setVelocity(velocity_i);
                particles[j].setVelocity(velocity_j);
            }
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}