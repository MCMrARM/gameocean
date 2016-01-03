#include "ItemAction.h"
#include "UseItemAction.h"
#include "DestroyBlockAction.h"

std::map<std::string, UseItemAction::Handler> UseItemAction::handlers;
std::map<std::string, DestroyBlockAction::Handler> DestroyBlockAction::handlers;