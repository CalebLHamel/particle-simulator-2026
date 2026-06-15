#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include "particle.hpp"
#include <thread>

// A qualities holder is anything we want to be able to get qualities from with an associated position.
// That is, chunks and superchunks.
class IQualitiesHolder {
    public:
        virtual ~IQualitiesHolder() {}

        virtual Qualities getQualities();
        virtual void updateQualities();

        virtual size_t getX();
        virtual size_t getY();
        virtual Vector2 getSimulationPosition();
};

// A chunk directly stores particles within itself, and is where a lot of coordination occurs.
class Chunk : public IQualitiesHolder {
    private:
        // A chunk holds particles within itself.
        std::vector<Particle> particles;
        
        // A chunk has collective qualities as a result of the a particles it contains.
        Qualities collective_qualities;
        
        // Where in the active list, if it is in an active list, it can be found.
        size_t active_index;
        
        // What other chunks are considered nearby (where individual particles matter).
        std::vector<Chunk*> nearby_chunks;

        // What other chunks are considered distant (where individual particles do not matter).
        std::vector<IQualitiesHolder*> distant_chunks;
        
        // Tracks where the chunk is in the simulation.
        size_t x;
        size_t y;
        
        // The spacial length of the chunk in the simulation space.
        float chunk_size;
        
        // How many divisions (of length) are made to form the subchunks.
        size_t chunk_divisions;
        
        // Subchunks are used in collisions, as they benefit from granular space as they are incredibly local effects.
        std::vector<std::vector<std::vector<Particle*>>> subchunks;
    
    public:
        // Constructor.
        Chunk(size_t x, size_t y, float chunk_size, size_t chunk_divisions);
        
        // Gets the X and Y chunk coordinate (an index).
        size_t getX();
        size_t getY();
        
        // Gets the particles present in one of the subchunks.
        std::vector<Particle*>* getParticlesInSubchunk(size_t x, size_t y);
        
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
        
        // Gets what other chunks are near to this chunk.
        std::vector<Chunk*>* getNearbyChunks();
        
        // Gets what other chunks are not near to this chunk.
        std::vector<IQualitiesHolder*>* getDistantChunks();
        
        // Gets where in the simulation the chunk is.
        Vector2 getSimulationPosition();
};

// Used to help further accelerate some force calculations.
class SuperChunk : public IQualitiesHolder {
    private:
        // What chunks does this super chunk hold?
        std::vector<Chunk*> sub_chunks;

        // What are the total qualities of each particle within this chunk?
        Qualities qualities;

        // The x and y coordinate of the superchunk in the superchunk grid.
        size_t x;
        size_t y;

        // The spacial width of the superchunk within the simulation.
        float chunk_size;

        // The spacial position of the superchunk within the simulation.
        Vector2 position;

    public:
        // Constructor.
        SuperChunk(size_t x, size_t y, float chunk_size);

        // Gets the qualities of this superchunk.
        Qualities getQualities();

        // Refreshes the qualities of the superchunk to match what it contains.
        void updateQualities();

        // Gets the X and Y index of the superchunk within the superchunk grid.
        size_t getX();
        size_t getY();

        // Gets where the superchunk is within the simulation.
        Vector2 getSimulationPosition();

        // Updates the position of the superchunk (to reflect the chunks it contains).
        // This should only be called once during setup per superchunk.
        void updatePosition();

        // Gets the chunks that the superchunk contains.
        std::vector<Chunk*>* getSubChunks();
};

// Used to run and organize the actual simulation.
class Simulation {
    private:
        // The spacial arrangement of chunks.
        std::vector<std::vector<Chunk>> chunks;
    
        // The spacial arrangement of superchunks.
        std::vector<std::vector<SuperChunk>> super_chunks;
        
        // For when the order of iteration doesn't matter, this only tracks chunks with particles in them.
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
        
        // How many particles the simulation contains.
        int count;

        // Places a particle within the simulation. Used for some internal logic.
        void placeParticle(Particle particle);

        // A helper function to determine forces that supports a thread.
        void threadDetermineForces(size_t start_inclusive, size_t end_exclusive);

        // How many additional threads will the simulation attempt to use at a maximum (constrained by hardware).
        size_t max_threads;

        // How many length divisions of a chunk to use to create the subchunks (used for collisions, not forces).
        size_t chunk_divisions;

    public:
        // Constructor.
        Simulation(size_t chunks_wide, size_t chunks_tall, float chunk_size, float local_radius, int max_collision_iterations, size_t max_threads, size_t chunk_divisions);

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