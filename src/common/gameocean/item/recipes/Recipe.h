#pragma once

#include <vector>
#include <map>
#include "../ItemInstance.h"

class ItemInstance;

class Recipe {

public:
    static int nextId;

    static std::map<int, Recipe *> recipes;

    const int id;
    std::vector<ItemInstance> result;
    std::map<ItemVariant *, ItemInstance> summedIngredients;

    Recipe(ItemInstance result) : id(nextId++) {
        recipes[id] = this;
        this->result.push_back(result);
    }

    virtual bool isShaped() { return false; }
    inline bool isShapeless() { return !isShaped(); }

    virtual void addIngredient(ItemInstance itm) {
        if (itm.isEmpty())
            return;
        if (summedIngredients.count(itm.getItem()) > 0) {
            summedIngredients[itm.getItem()].count += itm.count;
        } else {
            summedIngredients[itm.getItem()] = itm;
        }
    }

};

class ShapelessRecipe : public Recipe {

private:
    std::vector<ItemInstance> ingredients;

public:

    ShapelessRecipe(ItemInstance result) : Recipe(result) {
        //
    }

    virtual bool isShaped() {
        return false;
    }

    inline const std::vector<ItemInstance> &getIngredients() {
        return ingredients;
    }

    virtual void addIngredient(ItemInstance itm) {
        if (itm.isEmpty())
            return;
        Recipe::addIngredient(itm);
        ingredients.push_back(itm);
    }

};

class ShapedRecipe : public Recipe {

private:
    ItemInstance *ingredients;

public:
    const int sizeX;
    const int sizeY;

    ShapedRecipe(ItemInstance result, int sizeX, int sizeY) : Recipe(result), sizeX(sizeX), sizeY(sizeY) {
        ingredients = new ItemInstance[sizeX * sizeY];
    }

    virtual bool isShaped() {
        return true;
    }

    inline const ItemInstance *getIngredients() {
        return ingredients;
    }

    virtual void addIngredient(int x, int y, ItemInstance itm) {
        if (itm.isEmpty())
            return;
        Recipe::addIngredient(itm);
        ingredients[sizeY * y + x] = itm;
    }

};