#pragma once

#include <memory>
#include "../ActionHandlerData.h"
class BlockVariant;
class UseItemAction;
class DestroyBlockAction;

namespace Json { class Value; }

struct PlaceDoorHandlerData : public ActionHandlerData {
    BlockVariant *block = nullptr;
};
struct PlaceHalfHandlerData : public ActionHandlerData {
    BlockVariant *down = nullptr;
    BlockVariant *up = nullptr;
    BlockVariant *full = nullptr;
};
struct PlaceWithFacingHandlerData : public ActionHandlerData {
    BlockVariant *down = nullptr;
    BlockVariant *up = nullptr;
    BlockVariant *north = nullptr;
    BlockVariant *south = nullptr;
    BlockVariant *west = nullptr;
    BlockVariant *east = nullptr;
};

class DefaultActions {

protected:

    static bool throwSnowball(UseItemAction &action, ActionHandlerData *);
    static bool placeHalf(UseItemAction &action, ActionHandlerData *data);
    static std::unique_ptr<ActionHandlerData> processPlaceHalf(Json::Value const &value);
    static bool placeWithFacing(UseItemAction &action, ActionHandlerData *);
    static std::unique_ptr<ActionHandlerData> processPlaceWithFacing(Json::Value const &value);
    static std::unique_ptr<ActionHandlerData> processPlaceDoor(Json::Value const &value);
    static bool placeDoor(UseItemAction &action, ActionHandlerData *data);
    static bool openDoor(UseItemAction &action, ActionHandlerData *);
    static bool destroyDoor(DestroyBlockAction &action, ActionHandlerData *);

public:

    static void registerActions();

};


