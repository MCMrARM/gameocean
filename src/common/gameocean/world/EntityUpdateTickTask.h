#pragma once

#include <gameocean/utils/Thread.h>

class World;

class EntityUpdateTickTask : public Thread {

protected:
    bool shouldStop = false;

public:
    static int tickRate;

    World &world;

    EntityUpdateTickTask(World &world) : world(world) { }

    virtual void run();
    virtual void stop() { shouldStop = true; }

};


