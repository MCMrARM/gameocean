#pragma once

#include <vector>
#include <string>
#include "PlayerEvent.h"
#include "../CallbackList.h"
#include <gameocean/utils/Cancellable.h>

class ChatEvent : public PlayerEvent, public Cancellable {

public:
    static CallbackList<ChatEvent> callbacks;

protected:
    std::string message;
    const char* format;

public:
    ChatEvent(Player& player, std::string const& message, const char* format) : PlayerEvent(player), message(message), format(format) {
        //
    }

    inline std::string& getMessage() {
        return message;
    }

    inline void setMessage(std::string const& message) {
        this->message = message;
    }

    inline const char* getFormat() {
        return format;
    }

    inline void setFormat(const char* format) {
        this->format = format;
    }

};