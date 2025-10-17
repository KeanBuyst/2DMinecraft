#include "Crafting.h"

world::Crafting::Recipe::Recipe(const int size) : currentIndex(0), result(nullptr), size(size)
{
    items = new Material*[size];
    for (auto i = 0; i < size; ++i)
    {
        items[i] = new Material[size];
        for (auto j = 0; j < size; ++j)
        {
            items[i][j] = EMPTY;
        }
    }
}

world::Crafting::Recipe::Recipe(const Item* item, const int size) : currentIndex(0), result(item), size(size)
{
    items = new Material*[size];
    for (auto i = 0; i < size; ++i)
    {
        items[i] = new Material[size];
        for (auto j = 0; j < size; ++j)
        {
            items[i][j] = EMPTY;
        }
    }
}

world::Crafting::Recipe::Recipe(const Recipe& other) : currentIndex(other.currentIndex), size(other.size)
{
    items = new Material*[size];
    for (auto i = 0; i < size; ++i)
    {
        items[i] = new Material[size];
        for (auto j = 0; j < size; ++j)
        {
            items[i][j] = other.items[i][j];
        }
    }
    result = other.result ? new Item(*other.result) : nullptr;
}

world::Crafting::Recipe::~Recipe()
{
    for (auto i = 0; i < size; ++i)
    {
        delete[] items[i];
    }
    delete[] items;
    delete result;
}

world::Crafting::Recipe& world::Crafting::Recipe::setTemplate(Material** templ)
{
    for (auto i = 0; i < size; ++i)
    {
        delete[] items[i];
    }
    delete[] items;
    items = nullptr;

    items = templ;

    return *this;
}

world::Material& world::Crafting::Recipe::at(glm::ivec2 pos) const
{
    return items[pos.x % size][pos.y % size];
}

void world::Crafting::Recipe::add(Material ingredient)
{
    this->items[currentIndex % size][currentIndex / size] = ingredient;
    currentIndex++;
}

world::Item* world::Crafting::Recipe::getResult() const
{
    return new Item(*result);
}

int world::Crafting::Recipe::getSize() const
{
    return size;
}

world::Crafting::ShapedRecipe::ShapedRecipe(const Item* item,int size) : Recipe(item,size)
{}

bool world::Crafting::ShapedRecipe::matches(const Recipe& recipe) const
{
    if (size != recipe.getSize()) return false;
    for (auto x = 0; x < size; ++x)
    {
        for (auto y = 0; y < size; ++y)
        {
            if (items[x][y] != recipe.at({x,y}))
            {
                return false;
            }
        }
    }
    return true;
}

world::Crafting::ShapelessRecipe::ShapelessRecipe(const Item* item,int size) : Recipe(item,size)
{}

bool world::Crafting::ShapelessRecipe::matches(const Recipe& recipe) const
{
    if (size != recipe.getSize()) return false;
    const int capacity = size*size;
    int i1 = 0,i2 = 0;
    Material cache1[capacity];
    Material cache2[capacity];
    for (auto x = 0; x < size; ++x)
    {
        for (auto y = 0; y < size; ++y)
        {
            if (!recipe.at({x,y}).isEmpty())
            {
                cache1[i1] = recipe.at({x,y});
                i1++;
            }
            if (this->items[x][y].isEmpty())
            {
                cache2[i2] = this->items[x][y];
                i2++;
            }
        }
    }
    for (auto c1 = 0; c1 < capacity; ++c1)
    {
        bool notFound = true;
        for (auto c2 = 0; c2 < capacity; ++c2)
        {
            if (cache1[c1] == cache2[c2])
            {
                notFound = false;
                cache2[c2] = EMPTY;
                break;
            }
        }
        if (notFound)
            return false;
    }
    return true;
}

// Crafting interface

world::Crafting::Recipe** shaped_recipes;
constexpr int shaped_count = 2;
world::Crafting::Recipe** shapeless_recipes;
constexpr int shapeless_count = 0;

void world::Crafting::Init()
{
    Item* planks = new Item(OAK_PLANKS);
    planks->setAmount(4);

    shapeless_recipes = new Recipe*[shapeless_count];

    Material** crafting_table = new Material*[2] {
        new Material[2] { OAK_PLANKS, OAK_PLANKS },
        new Material[2] { OAK_PLANKS, OAK_PLANKS }
    };

    shaped_recipes = new Recipe*[shaped_count];
    shaped_recipes[0] = new ShapedRecipe(new Item(CRAFTING_TABLE), 2);
    shaped_recipes[0]->setTemplate(crafting_table);

    shaped_recipes[1] = new ShapedRecipe(planks, 1);
    shaped_recipes[1]->setTemplate(new Material*[1]{new Material(OAK_LOG)});
}

world::Item* world::Crafting::GetRecipe(const Recipe& recipe)
{
    for (auto i = 0; i < shaped_count; ++i)
    {
        if (shaped_recipes[i]->matches(recipe))
        {
            return shaped_recipes[i]->getResult();
        }
    }

    for (auto i = 0; i < shapeless_count; ++i)
    {
        if (shapeless_recipes[i]->matches(recipe))
        {
            return shapeless_recipes[i]->getResult();
        }
    }

    return nullptr;
}

void world::Crafting::Destroy()
{
    for (auto i = 0; i < shaped_count; ++i)
    {
        delete shaped_recipes[i];
    }
    delete[] shaped_recipes;
    for (auto i = 0; i < shapeless_count; ++i)
    {
        delete shapeless_recipes[i];
    }
    delete[] shapeless_recipes;
}