#pragma once

#include <chrono>
#include <vector>
#include <condition_variable>
#include <atomic>
#include "Thread.h"

class SchedulerThread : public Thread {

protected:
    virtual void run();

    long long nextTaskId = 0;

    struct ScheduledEntry {
        long long taskId;
        std::chrono::high_resolution_clock::time_point time;
        std::function<void ()> callback;
    };

    std::mutex notifyMutex;
    std::condition_variable notify;
    std::atomic<bool> shouldStop;
    std::vector<ScheduledEntry> tasks;

public:
    SchedulerThread() : Thread(), shouldStop(false) {
        //
    }

    virtual void stop();

    long long schedule(std::chrono::high_resolution_clock::duration time, std::function<void ()> callback);

    void cancelSchedule(long long id);

};


