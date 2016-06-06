#pragma once

#include <vector>
#include <gameocean/item/ItemInstance.h>
class Player;
class Inventory;

class InventoryTransaction {

public:
    Player &owner;

    InventoryTransaction(Player &owner) : owner(owner) {
        //
    }

    enum class InventoryKind {
        PLAYER, ARMOR, CONTAINER
    };

    struct Element {
        Inventory &inventory;
        InventoryKind kind;
        int slot;
        ItemInstance fromItem, toItem;

        int getDiff() const {
            if (fromItem.isEmpty() && toItem.isEmpty())
                return 0;
            if (fromItem.isEmpty())
                return toItem.count;
            if (toItem.isEmpty())
                return -fromItem.count;
            return toItem.count - fromItem.count;
        }
    };
    std::vector<Element> elements;

    bool isFinished();

    bool isValid();

    /**
     * It is possible that since the start of the transaction the inventory has changed and the transaction is not valid
     * anymore - in this case this function will return false.
     *
     * isValid calls this method as well.
     */
    bool isStillValid();

    bool isContainerTransfer();

    /**
     * Checks if the whole operation only happens in the user's inventory
     */
    bool isInInventoryTransfer();

    /**
     * Checks if the transfer happens between the inventory and armor slots
     */
    bool isArmorSlotTransfer();


    void execute();

    void revert();


    void reset();

};


