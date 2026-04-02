#pragma once
#include "UI.h"

#include "../Util.h"
#include "../crafting/Recipe.h"
#include "../world/entities/Item.h"

namespace UI
{
    class Inventory : public UIComponent
    {
    public:
        Inventory() = default;
        Inventory(const Inventory& other) = delete;
        ~Inventory() override;

        bool addItem(world::Item* item);
        void setItem(int index,world::Item* item);

        world::Item* getItem(int index);
        int getCount() const;

        void update() override;

        virtual void serialize(Util::ByteStream& stream);
        virtual void load(Util::ByteStream& stream);

        bool isVisible() const override;
        void setVisible(bool visible) override;
    protected:
        void DrawSlot(const world::Material& mat,int amount, glm::vec2 size = SLOT_SIZE);
        void CreateSlot(void** slot_ptr);

        world::Item** items;
        size_t count;

        bool updated;
        bool visible;
    };

    class Hotbar : public Inventory
    {
    private:
        int selected_slot;
    public:
        Hotbar();

        void render() override;
        void update() override;
        void setSelectedSlot(int slot);
        world::Item*& getSelectedItem() const;
    };

    class MouseItemHolder : public Inventory
    {
    private:
        Inventory* lastInv;
        int lastSlot;
    public:
        MouseItemHolder();
        void render() override;
        void setItem(world::Item* item);
        void saveLastSlot(Inventory* last,int slot);

        world::Item* getItem() const;
        void setLastSlot(world::Item*& item) const;
    };

    class PlayerInventory : public Inventory
    {
    private:
        int width,height;
        std::vector<crafting::Recipe> recipes;
        crafting::Ingredients ingredients;

        int crafting_shift;

        void UpdateRecipies();
    public:
        PlayerInventory();
        ~PlayerInventory() override;

        void setVisible(bool visible) override;

        void render() override;
        void update() override;
    };
}
