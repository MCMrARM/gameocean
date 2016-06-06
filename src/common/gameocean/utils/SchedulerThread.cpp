#include "SchedulerThread.h"
#include "Logger.h"

void SchedulerThread::stop() {
    shouldStop = true;
}

long long SchedulerThread::schedule(std::chrono::high_resolution_clock::duration time, std::function<void()> callback) {
    std::unique_lock<std::mutex> lock(notifyMutex);
    long long taskId = nextTaskId++;
    tasks.push_back({taskId, std::chrono::high_resolution_clock::now() + time, std::move(callback)});
    notify.notify_one();
    return taskId;
}

void SchedulerThread::cancelSchedule(long long id) {
    std::unique_lock<std::mutex> lock(notifyMutex);
    for (auto it = tasks.begin(); it != tasks.end();) {
        if (it->taskId == id)
            it = tasks.erase(it);
        else
            it++;
    }
}

void SchedulerThread::run() {
    bool waitForever = true;
    std::chrono::high_resolution_clock::time_point waitTime;
    std::vector<std::function<void ()>> executeQueue;
    while(true) {
        {
            std::unique_lock<std::mutex> lock(notifyMutex);
            if (!waitForever) {
                std::chrono::high_resolution_clock::duration duration =
                        waitTime - std::chrono::high_resolution_clock::now();
                if (duration.count() > 0)
                    notify.wait_for(lock, duration);
            } else {
                notify.wait(lock);
            }
            if (shouldStop) return;

            waitForever = true;
            for (auto it = tasks.begin(); it != tasks.end();) {
                if (it->time <= std::chrono::high_resolution_clock::now()) {
                    executeQueue.push_back(std::move(it->callback));
                    it = tasks.erase(it);
                } else {
                    if (waitForever) {
                        waitForever = false;
                        waitTime = it->time;
                    } else if (it->time < waitTime)
                        waitTime = it->time;
                    it++;
                }
            }
        }
        for (auto &exec : executeQueue) {
            exec();
        }
        executeQueue.clear();
    }
}