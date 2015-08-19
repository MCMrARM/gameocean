#include "Item.h"

std::map<std::string, Item*> Item::items = std::map<std::string, Item*>();
Item* Item::itemsByIds [256];