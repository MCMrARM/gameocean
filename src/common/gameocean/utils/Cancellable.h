#pragma once

class Cancellable {

protected:
    bool cancelled;

public:
    inline void cancel() {
        cancelled = true;
    }
    inline void setCancelled(bool val) {
        cancelled = val;
    }
    inline bool isCancelled() {
        return cancelled;
    }

};