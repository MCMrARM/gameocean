#include "entity/EntityDamageEvent.h"
#include "player/ChatEvent.h"
#include "player/PlayerJoinEvent.h"
#include "player/PlayerQuitEvent.h"
#include "player/PlayerMoveEvent.h"
#include "player/PlayerDamageEvent.h"
#include "player/PlayerAttackEvent.h"
#include "player/PlayerDeathEvent.h"

CallbackList<EntityDamageEvent> EntityDamageEvent::callbacks;
CallbackList<ChatEvent> ChatEvent::callbacks;
CallbackList<PlayerJoinEvent> PlayerJoinEvent::callbacks;
CallbackList<PlayerQuitEvent> PlayerQuitEvent::callbacks;
CallbackList<PlayerMoveEvent> PlayerMoveEvent::callbacks;
CallbackList<PlayerDamageEvent> PlayerDamageEvent::callbacks;
CallbackList<PlayerAttackEvent> PlayerAttackEvent::callbacks;
CallbackList<PlayerDeathEvent> PlayerDeathEvent::callbacks;