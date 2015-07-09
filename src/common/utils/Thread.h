#pragma once

#include <memory>
#include <thread>

class Thread {

protected:
    virtual void run() = 0;

public:
    std::thread* thread;

    Thread() {};
    ~Thread() { delete thread; };

    void start();

};


