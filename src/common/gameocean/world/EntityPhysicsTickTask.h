#pragma once

#include <gameocean/utils/Thread.h>

class World;

class EntityPhysicsTickTask : public Thread {

protected:
    bool shouldStop = false;

public:
    static int tickRate;

    World& world;

    EntityPhysicsTickTask(World& world) : world(world) { };

    virtual void run();
    virtual void stop() { shouldStop = true; };

};


