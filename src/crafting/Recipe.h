#pragma once
#include "../world/entities/Item.h"

namespace crafting
{
    struct Ingredient
    {
        uint16_t amount;
        world::Material item;
    };
    struct Ingredients
    {
        uint8_t amount;
        Ingredient* items;

        bool hasOverlap(const Ingredients& other) const;
    };
    struct Recipe
    {
        Ingredient result;
        Ingredients ingredients;
    };

    std::vector<Recipe> GetRecipe(const Ingredients& ingredients);
    Recipe GetRecipe(const world::Material& result);
}
