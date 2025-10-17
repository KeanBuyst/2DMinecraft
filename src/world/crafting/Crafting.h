#pragma once
#include "../entities/Item.h"

namespace world::Crafting
{
    class Recipe
    {
    private:
        int currentIndex;
    protected:
        Material** items;
        const Item* result;
        const int size;
    public:
        explicit Recipe(int size);
        explicit Recipe(const Item* item,int size);
        Recipe(const Recipe& other);

        virtual ~Recipe();
        virtual bool matches(const Recipe& recipe) const { return false; };

        Recipe& setTemplate(Material** templ);

        Material& at(glm::ivec2 pos) const;
        void add(Material ingredient);

        Item* getResult() const;
        int getSize() const;
    };

    class ShapedRecipe : public Recipe
    {
    public:
        explicit ShapedRecipe(const Item* item,int size);
        bool matches(const Recipe& recipe) const override;
    };

    class ShapelessRecipe : public Recipe
    {
    public:
        explicit ShapelessRecipe(const Item* item,int size);
        bool matches(const Recipe& recipe) const override;
    };

    void Init();
    Item* GetRecipe(const Recipe& recipe);
    void Destroy();
}
