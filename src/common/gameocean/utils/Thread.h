#pragma once

#include "../common.h"
#include <memory>
#include <thread>
#include <set>

class Thread {

protected:
    virtual void run() = 0;

public:
    static std::set<Thread *> threads;

    std::thread *thread;

    Thread() {
        threads.insert(this);
    }
    virtual ~Thread() {
        thread->join();
        delete thread;
        threads.erase(this);
    }

    void start();

    virtual void stop();

    inline bool isRunning() { return (thread != nullptr); }

};


