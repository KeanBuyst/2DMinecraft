#include "Recipe.h"

template <uint8_t N>
static constexpr uint8_t amountOf(const crafting::Ingredient (&)[N])
{
    return N;
}

static crafting::Ingredient CRAFTING_TABLE[] = {
    {
        4,
        world::OAK_PLANKS
    }
};
static crafting::Ingredient OAK_PLANKS[] = {
    {
        1,
        world::OAK_LOG
    }
};
static crafting::Ingredient STICK[] = {
    {
        1,
        world::OAK_PLANKS
    }
};

static crafting::Recipe recipes[] = {
    {
        {
            1,
            world::CRAFTING_TABLE
        },
        {
            amountOf(CRAFTING_TABLE),
            CRAFTING_TABLE
        }
    },
    {
        {
            4,
            world::OAK_PLANKS
        },
        {
            amountOf(OAK_PLANKS),
            OAK_PLANKS
        }
    },
    {
        {
            4,
            world::STICK
        },
        {
            amountOf(STICK),
            STICK
        }
    },
};

bool crafting::Ingredients::hasOverlap(const Ingredients& other) const
{
    for (auto i = 0; i < amount; ++i)
    {
        for (auto j = 0; j < other.amount; ++j)
        {
            if (items[i].item == other.items[j].item)
            {
                return true;
            }
        }
    }
    return false;
}

std::vector<crafting::Recipe> crafting::GetRecipe(const Ingredients& ingredients)
{
    std::vector<Recipe> collection;
    for (const auto& recipe : recipes)
    {
        if (recipe.ingredients.hasOverlap(ingredients))
        {
            collection.push_back(recipe);
        }
    }
    return collection;
}

crafting::Recipe crafting::GetRecipe(const world::Material& result)
{
    for (const auto& recipe : recipes)
    {
        if (recipe.result.item == result)
        {
            return recipe;
        }
    }
    return {{0,world::Material::EMPTY},{0,nullptr}};
}