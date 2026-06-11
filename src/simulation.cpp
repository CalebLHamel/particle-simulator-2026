#include "simulation.hpp"
#include "raymath.h"
#include <cstdio>
#include <algorithm>
#include <thread>

/**
 * Constructor method. Creates an empty chunk.
 */
Chunk::Chunk(size_t x, size_t y) {
    particles = std::vector<Particle>();
    Qualities collective_qualities = Qualities();
    this->x = x;
    this->y = y;

    this->nearby_chunks = std::vector<Chunk*>();
    this->distant_chunks = std::vector<Chunk*>();
    
    this->chunk_divisions = 5;
    this->subchunks = std::vector<std::vector<std::vector<Particle*>>>();
    for (size_t x=0; x<chunk_divisions; x+=1) {
        std::vector<std::vector<Particle*>> column = std::vector<std::vector<Particle*>>(); 
        for (size_t y=0; y<chunk_divisions; y+=1) {
            column.push_back(std::vector<Particle*>());
        }
        subchunks.push_back(column);
    }
}

/**
 * Updates the chunk's overall qualities based on the particles it contains.
 */
void Chunk::updateQualities(float chunksize) {
    // Clear present qualities.
    collective_qualities = Qualities();

    // Add the qualities of all the particles into the chunk's collective qualities.
    for (size_t i=0; i<particles.size(); i+=1) {
        collective_qualities.addQualities(particles[i].getQualities());
    }

    // Update subchunks.
    for (size_t x=0; x<chunk_divisions; x+=1) {
        for (size_t y=0; y<chunk_divisions; y+=1) {
            subchunks[x][y] = std::vector<Particle*>();
        }
    }
    for (size_t p=0; p<particles.size(); p+=1) {
        Particle* particle = &particles[p];
        size_t sub_x = std::max((size_t)0, std::min((chunk_divisions-1), (size_t)floor(((size_t)(particle->getPosition().x * chunk_divisions / chunksize)) % chunk_divisions)));
        size_t sub_y = std::max((size_t)0, std::min((chunk_divisions-1), (size_t)floor(((size_t)(particle->getPosition().y * chunk_divisions / chunksize)) % chunk_divisions)));
        
        subchunks[sub_x][sub_y].push_back(particle);
    }
}

/**
 * Gets the collective qualities of a chunk.
 */
Qualities Chunk::getQualities() {
    return this->collective_qualities;
}

/**
 * Gets the particles within this chunk.
 */
std::vector<Particle>* Chunk::getParticles() {
    return &particles;
}

/**
 * Gets the active index of the chunk.
 * This only has meaning if it is currently in an active list, managed by the simulation.
 */
size_t Chunk::getActiveIndex() {
    return active_index;
}

/**
 * Sets the active index of the chunk. This is managed by the simulation.
 */
void Chunk::setActiveIndex(size_t index) {
    active_index = index;
}

/**
 * Pops a particle out of the chunk.
 */
Particle Chunk::popParticle(size_t index) {
    // Get the particle.
    Particle particle = particles[index];

    // Move the last particle where the popped particle was, if applicable.
    if (index+1 < particles.size()) {
        particles[index] = particles.back();
    }
    // Either the popped particle was last, or the last particle was duplicated, either way, remove the last item.
    particles.pop_back();

    // Finally, return the particle.
    return particle;
}

/**
 * Gets the x chunk coordinate of the chunk.
 */
size_t Chunk::getX() {
    return x;
}

/**
 * Gets the y chunk coordinate of the chunk.
 */
size_t Chunk::getY() {
    return y;
}

/**
 * Gets access to the nearby chunks for the chunk.
 */
std::vector<Chunk*>* Chunk::getNearbyChunks() {
    return &(this->nearby_chunks);
}

/**
 * Gets access to the distant chunks for the chunk.
 */
std::vector<Chunk*>* Chunk::getDistantChunks() {
    return &(this->distant_chunks);
}

std::vector<Particle*>* Chunk::getParticlesInSubchunk(size_t x, size_t y) {
    return &subchunks[x][y];
}

/**
 * Constructor for the simulation.
 * chunks_wide, chunks_tall : Specify how many chunks wide and tall the simulation is.
 * chunk_size               : Specifies how large the chunks in the simulation are.
 * local_radius             : Specifies the distance at which the particle forces should still be calculated on a per-particle basis.
 * max_collision_iterations : Specifies the limit of how many iterations the collision solver is allowed to process.
 */
Simulation::Simulation(size_t chunks_wide, size_t chunks_tall, float chunk_size, float local_radius, int max_collision_iterations) {
    // No chunks are active to begin with.
    active_chunks = std::vector<Chunk*>();
    chunk_divisions = 5;
    // Make new 2D vector to hold the chunks.
    chunks = std::vector<std::vector<Chunk>>();

    // For every column that should exist.
    for (size_t x=0; x<chunks_wide; x+=1) {
        // Make the column.
        std::vector<Chunk> column = std::vector<Chunk>();
        // Populate the column with new, empty chunks. 
        for (size_t y=0; y<chunks_wide; y+=1) {
            column.push_back(Chunk(x, y));
        }
        // Add the column to the chunks.
        chunks.push_back(column);
    }

    this->count = 0;

    // Set remaining fields.
    this->chunk_size = chunk_size;
    this->local_radius = local_radius;
    this->max_collision_iterations = max_collision_iterations;
    this->chunks_wide = chunks_wide;
    this->chunks_tall = chunks_tall;

    float local_radius_squared = local_radius*local_radius;
    // For each chunk.
    for (size_t x = 0; x < chunks_wide; x+=1) {
        for (size_t y = 0; y < chunks_tall; y+=1) {
            Chunk* chunk = &chunks[x][y];

            // Get the nearby and distant chunk lists.
            std::vector<Chunk*>* nearby = chunk->getNearbyChunks();
            std::vector<Chunk*>* distant = chunk->getDistantChunks();

            // For every other chunk.
            for (size_t x2 = 0; x2 < chunks_wide; x2+=1) {
                for (size_t y2 = 0; y2 < chunks_tall; y2 += 1) {
                    if (x2 == x && y2 == y) {
                        continue;
                    }

                    // Determine how far it is.
                    float distance_squared = ((x2-x)*(x2-x) + (y2-y)*(y2-y)) * chunk_size*chunk_size;
                    
                    // If within the local radius, put it in the nearby list, else in the distant list.
                    if (distance_squared < local_radius_squared) {
                        nearby->push_back(&chunks[x2][y2]);
                    } else {
                        distant->push_back(&chunks[x2][y2]);
                    }
                }
            }
        }
    }

    
    unsigned int cores = std::thread::hardware_concurrency();
    threads_available = 6;
    printf("\nAvailable hardware threads: %d\n\n", cores);
}

/**
 * Places a particle in the simulation.
 */
void Simulation::placeParticle(Particle particle) {
    // Get the particle position.
    Vector2 position = particle.getPosition();

    // Divide by chunk size and floor to get chunk coordinates.
    int x = floor(position.x / chunk_size);
    int y = floor(position.y / chunk_size);

    x = std::max(0, std::min(x, (int)chunks_wide-1));
    y = std::max(0, std::min(y, (int)chunks_tall-1));

    // Add particle to the chunks.
    chunks[x][y].getParticles()->push_back(particle);

    // We just added a particle to the chunk, so it should be added to active if not already there.
    ensureChunkIsActive(&chunks[x][y]);
}

/**
 * Adds a particle to the simulation.
 */
void Simulation::addParticle(Particle particle) {
    count += 1;
    placeParticle(particle);
}

/**
 * Gets the total number of particles in the simulation.
 */
int Simulation::getCount() {
    return count;
}

/**
 * Gets all of the particle in a chunk.
 */
std::vector<Particle>* Simulation::getParticlesInChunk(int x, int y) {
    return chunks[x][y].getParticles();
}

/**
 * Moves all the particles in the simulation, automatically managing the chunks.
 */
void Simulation::moveParticles() {
    // Move the particles in each chunk.
    for (size_t c=0; c<active_chunks.size(); c+=1) {
        for (size_t p=0; p< active_chunks[c]->getParticles()->size(); p+=1) {
            Particle* particle = &((*(active_chunks[c]->getParticles()))[p]);
            Vector2 old_position = particle->getPosition();
            Vector2 velocity = particle->getVelocity();
            Vector2 new_position = Vector2Add(old_position, velocity);
            new_position.x = std::max(particle->getRadius(), std::min(new_position.x, chunks_wide*chunk_size - particle->getRadius()));
            new_position.y = std::max(particle->getRadius(), std::min(new_position.y, chunks_tall*chunk_size - particle->getRadius()));
            particle->setPosition(new_position);
        }
    }

    // Move particles between chunks as needed.
    // For every active chunk.
    size_t c = 0;
    while (c < active_chunks.size()) {
        Chunk* chunk = active_chunks[c];
        
        // For every particle in the active chunk.
        size_t p = 0;
        while (p < chunk->getParticles()->size()) {
            std::vector<Particle>* particles = chunk->getParticles();
            Particle particle = (*particles)[p];

            // Get the particle's position.
            Vector2 position = particle.getPosition();

            // Divide by chunk size and floor to get chunk coordinates.
            int x = floor(position.x / chunk_size);
            int y = floor(position.y / chunk_size);

            // If the particle no-longer fits in the chunk, move it.
            if (x != chunk->getX() || y != chunk->getY()) {
                chunk->popParticle(p);
                
                placeParticle(particle);
            // Otherwise, advance through the particles list.
            } else {
                p += 1;
            }
        }

        // If the chunk is no-longer active, remove it from the active list.
        if (chunk->getParticles()->size() == 0) {
            // Make sure the chunk is inactive, and if it was inactive to begin with,
            if (ensureChunkIsInactive(chunk)) {
                // it means we can check the next active chunk index, since nothing from active_chunks was removed.
                c += 1;
            }  // Else, chunk was made inactive, therefore removed from list, so we don't progress the index.
        // If the chunk is still active, we can check the next active chunk index.
        } else {
            c += 1;
        }
    }

    updateChunkQualities();
}

/**
 * Ensures that a chunk is active.
 * Returns true if the chunk is already active.
 * Returns false if the chunk wasn't active, and makes it active in response.
 */
bool Simulation::ensureChunkIsActive(Chunk* chunk) {
    // If the chunk is not in the active index list.
    if (chunk->getActiveIndex() >= active_chunks.size() || active_chunks[chunk->getActiveIndex()] != chunk) {
        // Update the chunk's active index.
        chunk->setActiveIndex(active_chunks.size());
        // Add the chunk to the active list.
        active_chunks.push_back(chunk);
        return false;
    }
    return true;
}

/**
 * Ensures that a chunk is inactive.
 * Returns true if the chunk is already inactive.
 * Returns false if the chunk wasn't inactive, and makes it inactive in response.
 */
bool Simulation::ensureChunkIsInactive(Chunk* chunk) {
    // If the chunk is in the active index list.
    if (chunk->getActiveIndex() < active_chunks.size() && active_chunks[chunk->getActiveIndex()] == chunk) {
        // If the chunk isn't the last one in the list.
        if (chunk->getActiveIndex() != active_chunks.size()) {
            // Move the last chunk in the list to its position.
            Chunk* last_chunk = active_chunks.back();
            last_chunk->setActiveIndex(chunk->getActiveIndex());
            active_chunks[chunk->getActiveIndex()] = last_chunk;
        }
        // Regardless of if the last one was copied over, remove the last chunk in the list.
        active_chunks.pop_back();
        return false;
    }
    return true;
}

void Simulation::determineForces() {
    //updateChunkQualities();

    std::vector<std::thread> threads = std::vector<std::thread>();
    size_t chunks_available = active_chunks.size();
    size_t per_thread = (size_t) ceil(((double)chunks_available) / (double)threads_available);
    for (size_t i=0; i<threads_available; i+=1) {
        threads.push_back(std::thread(&Simulation::threadDetermineForces, *this, i*per_thread, std::min((i+1)*per_thread,chunks_available)));
    }
    for (size_t i=0; i<threads.size(); i+=1) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }
}

/**
 * Calculate all the forces on particles, and how they accelerate.
 * Updates their velocities based on these accelerations.
 */
void Simulation::threadDetermineForces(size_t start_inclusive, size_t end_exclusive) {
    // For each chunk with particles in it.
    for (size_t c = start_inclusive; c < end_exclusive; c += 1) {
        Chunk* chunk = active_chunks[c];

        // Get the chunk's coordinates.
        size_t x = chunk->getX();
        size_t y = chunk->getY();

        // Get the nearby and distant chunk lists for the chunk.
        std::vector<Chunk*>* nearby = chunk->getNearbyChunks();
        std::vector<Chunk*>* distant = chunk->getDistantChunks();
    
        // Tracks the influence of external forces, missing the effects of the particles they apply to.
        Vector2 partial_force_vector_sum = {0,0};

        // For every distant chunk.
        for (size_t d=0; d<distant->size(); d+=1) {
            Chunk* distant_chunk = (*distant)[d];

            // Get the distant chunk's qualities and position.
            Qualities qualities = distant_chunk->getQualities();
            size_t x2 = distant_chunk->getX();
            size_t y2 = distant_chunk->getY();

            // Compute the force effects from the chunk.
            Vector2 position_difference = Vector2Subtract(Vector2{(float)x2,(float)y2}, Vector2{(float)x,(float)y});
            position_difference = Vector2Scale(position_difference, chunk_size);
            float distance_squared = Vector2LengthSqr(position_difference);
            Vector2 direction = Vector2Normalize(position_difference);
            partial_force_vector_sum = Vector2Add(partial_force_vector_sum, Vector2Scale(direction, -qualities.getQuality(Charge)/distance_squared));
        }

        // For every particle in this chunk.
        for (size_t p=0; p<chunk->getParticles()->size(); p+=1) {
            Particle* particle = &(*chunk->getParticles())[p];

            // Get the particle's position and qualities.
            Vector2 particle_position = particle->getPosition();
            Qualities qualities = particle->getQualities();

            // The net force on the particle starts with the partial force vector sum from the chunks found earlier, scaled by the appropriate quality.
            Vector2 net_force = Vector2Scale(partial_force_vector_sum, qualities.getQuality(Charge));

            // For every other nearby chunk.
            for (size_t n=0; n<nearby->size(); n+=1) {
                Chunk* nearby_chunk = (*nearby)[n];
                // For every particle it contains.
                std::vector<Particle>* nearby_particles = nearby_chunk->getParticles();
                for (size_t np=0; np<nearby_particles->size(); np += 1) {
                    Particle* nearby_particle = &(*nearby_particles)[np];
                    Vector2 nearby_particle_position = nearby_particle->getPosition();
                    Qualities nearby_qualities = nearby_particle->getQualities();

                    // Calculate its effect on the force and add it to the net force.
                    Vector2 position_difference = Vector2Subtract(nearby_particle_position, particle_position);
                    float distance_squared = Vector2LengthSqr(position_difference);
                    if (distance_squared < (particle->getRadius()+nearby_particle->getRadius())*(particle->getRadius()+nearby_particle->getRadius())) {
                        continue;
                    }
                    Vector2 direction = Vector2Normalize(position_difference);
        
                    net_force = Vector2Add(net_force, Vector2Scale(direction, -qualities.getQuality(Charge)*nearby_qualities.getQuality(Charge)/distance_squared));
                }
            }
            // For every other particle in this chunk.
            // For every other nearby chunk.
            for (size_t n=0; n<chunk->getParticles()->size(); n+=1) {
                // Skip over self.
                if (n == p) {
                    continue;
                }
                Particle* nearby_particle = &(*chunk->getParticles())[n];
                Vector2 nearby_particle_position = nearby_particle->getPosition();
                Qualities nearby_qualities = nearby_particle->getQualities();

                Vector2 position_difference = Vector2Subtract(nearby_particle_position, particle_position);
                float distance_squared = Vector2LengthSqr(position_difference);
                if (distance_squared < (particle->getRadius()+nearby_particle->getRadius())*(particle->getRadius()+nearby_particle->getRadius())) {
                    continue;
                }

                Vector2 direction = Vector2Normalize(position_difference);
                
                net_force = Vector2Add(net_force, Vector2Scale(direction, -qualities.getQuality(Charge)*nearby_qualities.getQuality(Charge)/distance_squared));
            }

            Vector2 acceleration = Vector2Scale(net_force, 1/qualities.getQuality(Mass));
            particle->setVelocity(Vector2Add(particle->getVelocity(), acceleration));
        }
    }
}

/**
 * Detects and deals with collisions that particles experience.
 * It does change their position, but it doesn't update their position in the chunks, since
 * the movement in this stage for particles should be small enough to stay in near enough to their
 * chunks to not mess with the rest of the program.
 * The velocies and extremes needed to break this mean that the rest of the simulation is going to break
 * anyways.
 */
void Simulation::manageCollisions() {
    // How many attepts to resolve the collisions remain this call.
    size_t loops_remaining = max_collision_iterations;
    
    // A helper variable. We run the loop until there are no more collisions to resolve.
    bool collision_detected = true;

    // While there may be collisions to resolve, and the max loops haven't been exceeded.
    while (collision_detected && loops_remaining > 0) {
        loops_remaining -= 1;
        
        // Now that we're in the loop, assume there are no collisions until it's potentially found otherwise.
        collision_detected = false;

        // For every active chunk.
        for (size_t c=0; c<active_chunks.size(); c+=1) {
            Chunk* chunk = active_chunks[c];

            // For every particle in the chunk.
            for (size_t p=0; p<chunk->getParticles()->size(); p+=1) {
                std::vector<Particle>* particles = chunk->getParticles();
                Particle* particle = &((*particles)[p]);
                float x = particle->getPosition().x;
                float y = particle->getPosition().y;

                // Bounce it off of the wall if it has to.
                // Wall dampening can be used to reduce a particle's speed on striking a wall.
                float wall_dampening = 0.9;
                Vector2 vel = particle->getVelocity();
                float x_vel = vel.x;
                float y_vel = vel.y;
                float radius = particle->getRadius();
 
                // If beyond the chunk boundaries, and actively moving away from them, bounce them back.
                if ((x >= chunk_size*chunks_wide - particle->getRadius() && x_vel > 0) || (x <= particle->getRadius() && x_vel < 0)) {
                    vel.x *= -wall_dampening;
                    particle->setVelocity(vel);
                    collision_detected = true;
                }
                if ((y >= chunk_size*chunks_tall - particle->getRadius() && y_vel > 0) || (y <= particle->getRadius() && y_vel < 0)) {
                    vel.y *= -wall_dampening;
                    particle->setVelocity(vel);
                    collision_detected = true;
                }

                // Make sure that the chunk coordinates stay within limits. At times, some faster particles could squeak out enough to cause issues.
                //size_t chunk_coord_x = std::max(0, std::min((int)floor(x / (chunk_size*chunk_divisions)), (int)(chunks_wide*chunk_divisions)-1));
                //size_t chunk_coord_y = std::max(0, std::min((int)floor(y / (chunk_size*chunk_divisions)), (int)(chunks_tall*chunk_divisions)-1));
                size_t chunk_coord_x = std::max((size_t)0, std::min(chunks_wide*chunk_divisions-1, (size_t)floor(((size_t)(x * chunk_divisions / chunk_size)))));
                size_t chunk_coord_y = std::max((size_t)0, std::min(chunks_tall*chunk_divisions-1, (size_t)floor(((size_t)(y * chunk_divisions / chunk_size)))));


                // The "other chunk" refers to the chunk that the other particles we check against are in. It will most-likely be the same chunk for some of them.
                size_t other_chunk_coord_y = chunk_coord_y;
                // If the particle is above the midpoint, the other chunk coords will tend upwards, and vice versa.
                if (y > chunk_coord_y*chunk_size/chunk_divisions + (chunk_size/(chunk_divisions*2.0))) {
                    other_chunk_coord_y = (chunk_coord_y + 1 == chunks_tall*chunk_divisions) ? chunk_coord_y : chunk_coord_y + 1;
                } else {
                    other_chunk_coord_y = (chunk_coord_y == 0) ? chunk_coord_y : chunk_coord_y - 1;
                }
                size_t other_chunk_coord_x = chunk_coord_x;
                // If the particle is to the right of the midpoint, the chunk coords will tend right, and vice versa.
                if (x > chunk_coord_x*chunk_size/chunk_divisions+(chunk_size/(chunk_divisions*2.0))) {
                    other_chunk_coord_x = (chunk_coord_x + 1 == chunks_wide*chunk_divisions) ? chunk_coord_x : chunk_coord_x + 1;
                } else {
                    other_chunk_coord_x = (chunk_coord_x == 0) ? chunk_coord_x : chunk_coord_x - 1;
                }

                // Iterate over the original chunk coordinate, and the three nearest chunks as found.
                size_t pairs[4][2] = {{chunk_coord_x,chunk_coord_y}, {chunk_coord_x, other_chunk_coord_y}, {other_chunk_coord_x, chunk_coord_y}, {other_chunk_coord_x, other_chunk_coord_y}};
                for (size_t i=0; i<4; i+=1) {
                    other_chunk_coord_x = pairs[i][0];
                    other_chunk_coord_y = pairs[i][1];

                    // If looking at another chunk, and it is the same one (this is set up to happen if at a boundary), skip it.
                    if (i != 0 && chunk_coord_x == other_chunk_coord_x && chunk_coord_y == other_chunk_coord_y) {
                        continue;
                    }

                    
                    // Get the other chunk.
                    Chunk* other_chunk = &chunks[other_chunk_coord_x/chunk_divisions][other_chunk_coord_y/chunk_divisions];
                    std::vector<Particle*>* other_particles = other_chunk->getParticlesInSubchunk(other_chunk_coord_x%chunk_divisions, other_chunk_coord_y%chunk_divisions);
                    // For every other particle in the chunk.
                    
                    for (size_t op=0; op < other_particles->size(); op+=1 ) {
                        // Get the other particle that we're checking against.
                        Particle* other_particle = (*other_particles)[op];

                        // Get the difference in their positions.
                        Vector2 position_difference = Vector2Subtract(particle->getPosition(), other_particle->getPosition());
                        if (position_difference.x == 0 && position_difference.y == 0) {
                            continue;
                        }

                        // If they are too far away to be colliding, skip it, and check the next particle.
                        if (Vector2Length(position_difference) >= particle->getRadius() + other_particle->getRadius()) {
                            continue;
                        }

                        // Motion is relative, so we treat the collision as the particle having all the motion and the other particle being stationary.
                        Vector2 velocity_relative_to_other = Vector2Subtract(particle->getVelocity(), other_particle->getVelocity());
                        
                        // Get the direction from the other particle to this one.
                        Vector2 position_difference_direction = Vector2Normalize(position_difference);

                        // The dot product of the position difference and the relative velocity tells us how direct of an impact it is.
                        float impact_speed = Vector2DotProduct(position_difference_direction, velocity_relative_to_other);
                    
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
                        
                        float mass = particle->getQuality(Mass);
                        float other_mass = other_particle->getQuality(Mass);
                        
                        // These derive from simplifying the equations for perfectly elastic collisions.
                        Vector2 change_in_velocity = Vector2Subtract(Vector2Scale(impact_velocity, (mass-other_mass)/(mass+other_mass)), impact_velocity);
                        Vector2 change_in_other_velocity = Vector2Scale(impact_velocity, (2*mass)/(mass+other_mass));
                        
                        Vector2 temp = particle->getVelocity();
                        // The particles' final velocities are their initial velocities, plus their change in velocity.
                        particle->setVelocity(Vector2Add(particle->getVelocity(), change_in_velocity));
                        other_particle->setVelocity(Vector2Add(other_particle->getVelocity(), change_in_other_velocity));                        

                        // Update where the particles would end up based on the velocity changes from the collision.
                        // This is why the main loop has to interate until no collisions cause a change, because a change
                        // in the positions as the result of collisions may incur more collisions.

                        particle->setPosition(particle->getPriorPosition()+particle->getVelocity());
                        other_particle->setPosition(other_particle->getPriorPosition()+other_particle->getVelocity());    
                    }
                }
            }
        }
    }

    // Set every particle's prior position to their current position.
    // This prepares them for following computations by remembering where they were.
    for (size_t c=0; c<active_chunks.size(); c+=1) {
        Chunk* chunk = active_chunks[c];
        for (size_t p=0; p<chunk->getParticles()->size(); p+=1) {
            Particle* particle = &(*chunk->getParticles())[p];
            particle->readyForNewPosition();
        }
    }
}

void Simulation::updateChunkQualities() {
    for (size_t x=0; x<chunks_wide; x+=1) {
        for (size_t y=0; y<chunks_wide; y+=1) {
            chunks[x][y].updateQualities(chunk_size);
        }
    }
}

