#include "player/ChatEvent.h"
#include "player/PlayerJoinEvent.h"
#include "player/PlayerQuitEvent.h"

CallbackList<ChatEvent> ChatEvent::callbacks;
CallbackList<PlayerJoinEvent> PlayerJoinEvent::callbacks;
CallbackList<PlayerQuitEvent> PlayerQuitEvent::callbacks;