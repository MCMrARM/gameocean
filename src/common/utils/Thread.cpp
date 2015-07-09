#include "Thread.h"

#include <functional>

void Thread::start() {
    thread = new std::thread(&Thread::run, this);
}