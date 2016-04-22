#pragma once

#include <memory>
#include "../ActionHandlerData.h"
class BlockVariant;
class UseItemAction;

namespace Json { class Value; }

struct PlaceHalfHandlerData : public ActionHandlerData {
    BlockVariant* down = nullptr;
    BlockVariant* up = nullptr;
    BlockVariant* full = nullptr;
};
struct PlaceWithFacingHandlerData : public ActionHandlerData {
    BlockVariant* down = nullptr;
    BlockVariant* up = nullptr;
    BlockVariant* north = nullptr;
    BlockVariant* south = nullptr;
    BlockVariant* west = nullptr;
    BlockVariant* east = nullptr;
};

class DefaultActions {

protected:

    static bool throwSnowball(UseItemAction& action, ActionHandlerData*);
    static bool placeHalf(UseItemAction& action, ActionHandlerData*);
    static std::unique_ptr<ActionHandlerData> processPlaceHalf(Json::Value const& value);
    static bool placeWithFacing(UseItemAction& action, ActionHandlerData*);
    static std::unique_ptr<ActionHandlerData> processPlaceWithFacing(Json::Value const& value);

public:

    static void registerActions();

};


