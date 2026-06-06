#include "simulation.h"
#include "raymath.h"
#include <cstdio>
#include <algorithm>

/**
 * Constructor method. Creates an empty chunk.
 */
Chunk::Chunk(size_t x, size_t y) {
    particles = std::vector<Particle>();
    Qualities collective_qualities = Qualities();
    this->x = x;
    this->y = y;
}

/**
 * Updates the chunk's overall qualities based on the particles it contains.
 */
void Chunk::updateQualities() {
    // Clear present qualities.
    collective_qualities = Qualities();

    // Add the qualities of all the particles into the chunk's collective qualities.
    for (size_t i=0; i<particles.size(); i+=1) {
        collective_qualities.addQualities(particles[i].getQualities());
    }
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
 * Constructor for the simulation.
 * chunks_wide, chunks_tall : Specify how many chunks wide and tall the simulation is.
 * chunk_size               : Specifies how large the chunks in the simulation are.
 * local_radius             : Specifies the distance at which the particle forces should still be calculated on a per-particle basis.
 * max_collision_iterations : Specifies the limit of how many iterations the collision solver is allowed to process.
 */
Simulation::Simulation(size_t chunks_wide, size_t chunks_tall, float chunk_size, float local_radius, int max_collision_iterations) {
    // No chunks are active to begin with.
    active_chunks = std::vector<Chunk*>();
    
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

/**
 * Calculate all the forces on particles, and how they accelerate.
 * Updates their velocities based on these accelerations.
 */
void Simulation::determineForces() {
    // TODO
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
                size_t chunk_coord_x = std::max(0, std::min((int)floor(x / chunk_size), (int)chunks_wide-1));
                size_t chunk_coord_y = std::max(0, std::min((int)floor(y / chunk_size), (int)chunks_tall-1));

                // The "other chunk" refers to the chunk that the other particles we check against are in. It will most-likely be the same chunk for some of them.
                size_t other_chunk_coord_y = chunk_coord_y;
                // If the particle is above the midpoint, the other chunk coords will tend upwards, and vice versa.
                if (y > chunk->getY()+(chunk_size/2)) {
                    other_chunk_coord_y = (chunk_coord_y + 1 == chunks_tall) ? chunk_coord_y : chunk_coord_y + 1;
                } else {
                    other_chunk_coord_y = (chunk_coord_y == 0) ? chunk_coord_y : chunk_coord_y - 1;
                }
                size_t other_chunk_coord_x = chunk_coord_x;
                // If the particle is to the right of the midpoint, the chunk coords will tend right, and vice versa.
                if (x > chunk->getX()+(chunk_size/2)) {
                    other_chunk_coord_x = (chunk_coord_x + 1 == chunks_wide) ? chunk_coord_x : chunk_coord_x + 1;
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
                    Chunk* other_chunk = &chunks[other_chunk_coord_x][other_chunk_coord_y];

                    // For every other particle in the chunk.
                    // We skip over particles in the current chunk that would have been covered already.
                    for (size_t op=( (i==0) ? p+1 : 0 ); op < other_chunk->getParticles()->size(); op+=1 ) {
                        // Get the other particle that we're checking against.
                        std::vector<Particle>* other_particles = other_chunk->getParticles(); 
                        Particle* other_particle = &((*other_particles)[op]);

                        // Get the difference in their positions.
                        Vector2 position_difference = Vector2Subtract(particle->getPosition(), other_particle->getPosition());

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