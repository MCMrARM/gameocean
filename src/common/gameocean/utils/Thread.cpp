#include "Thread.h"

#include <functional>
#include <gameocean/utils/Logger.h>

std::set<Thread*> Thread::threads = std::set<Thread*>();

void Thread::start() {
    thread = new std::thread(&Thread::run, this);
}

void Thread::stop() {
    Logger::main->warn("Thread", "This thread won't be stopped because it doesn't implement stop()");
}