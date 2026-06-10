#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include "particle.hpp"
#include <thread>

class Chunk {
private:
    // A chunk holds particles within itself.
    std::vector<Particle> particles;

    // A chunk has collective qualities as a result of the a particles it contains.
    Qualities collective_qualities;

    // Where in the active list, if it is in an active list, it can be found.
    size_t active_index;

    std::vector<Chunk*> nearby_chunks;
    std::vector<Chunk*> distant_chunks;

    // Tracks where the chunk is in the simulation.
    size_t x;
    size_t y;

public:
    // Constructor.
    Chunk(size_t x, size_t y);

    size_t getX();
    size_t getY();

    // Tells the chunk to determine its overall qualities from its contents.
    void updateQualities();
    Qualities getQualities();
    
    // Gets the list of all particles in the chunk.
    std::vector<Particle>* getParticles();

    // Getter and setter for the active index.
    size_t getActiveIndex();
    void setActiveIndex(size_t index);

    // Pops a particle out of the chunk.
    Particle popParticle(size_t index);

    std::vector<Chunk*>* getNearbyChunks();
    std::vector<Chunk*>* getDistantChunks();
};

class Simulation {
    private:
        // The spatial arrangement of chunks.
        std::vector<std::vector<Chunk>> chunks;

        // For when the order of iteration doesn't matter, which is often, this only tracks chunks with particles in them.
        std::vector<Chunk*> active_chunks;

        // What length to use for the chunks.
        float chunk_size;

        // For forces, how close is close enough to calculate on a per-particle basis?
        float local_radius;
        
        // How many attempts to resolve collisions can be taken per call?
        int max_collision_iterations;

        // How large the simulation space is.
        size_t chunks_wide;
        size_t chunks_tall;
        
        // How many particles does the simulation contain.
        int count;

        // Places a particle within the simulation. Used for some internal logic.
        void placeParticle(Particle particle);

        void threadDetermineForces(size_t start_inclusive, size_t end_exclusive);

        int threads_available;

    public:
        // Constructor.
        Simulation(size_t chunks_wide, size_t chunks_tall, float chunk_size, float local_radius, int max_collision_iterations);

        // Adds a particle to the simulation.
        void addParticle(Particle particle);

        // Gets the particle count.
        int getCount();

        // Determines forces on all particles and applies acceleration.
        void determineForces();

        // Manages collisions.
        void manageCollisions();

        // Gets all the particles in a chunk.
        std::vector<Particle>* getParticlesInChunk(int x, int y);

        // Moves all the particles.
        void moveParticles();

        // Adds the given chunk to the active list if it isn't already there.
        bool ensureChunkIsActive(Chunk* chunk);

        // Removes the given chunk from the active list if it isn't already gone.
        bool ensureChunkIsInactive(Chunk* chunk);

        // Updates each chunk's tracking of qualities.
        void updateChunkQualities();
};

#endif