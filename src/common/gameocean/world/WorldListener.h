#pragma once

class World;
class Chunk;

class WorldListener {

public:
    /**
     * This method is called when a World is being destroyed.
     */
    virtual void onWorldDestroy(World &world) { }

    /**
     * This method is called as soon as a Chunk is added to the world. It won't be ready for access and won't contain
     * block data.
     */
    virtual void onChunkAdded(World &world, Chunk &chunk) { }

    /**
     * This method is called after a Chunk is ready to be used.
     */
    virtual void onChunkLoaded(World &world, Chunk &chunk) { }

    /**
     * This method is called when a Chunk is being removed from World. This function is given a Chunk reference which will
     * be deleted just after the method call.
     */
    virtual void onChunkUnloaded(World &world, Chunk &chunk) { }

};