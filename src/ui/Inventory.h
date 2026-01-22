#pragma once
#include "UI.h"

#include "../Util.h"

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

        virtual void serialize(Util::ByteStream& stream);
        virtual void load(Util::ByteStream& stream);

        void update() override;

        bool isVisible() const override;
        void setVisible(bool visible) override;
    protected:
        void DrawSlot(void** slot_ptr);

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
        void update() override;
        void setItem(world::Item* item);
        void saveLastSlot(Inventory* last,int slot);

        world::Item* getItem() const;
        void setLastSlot(world::Item*& item) const;
    };

    class PlayerInventory : public Inventory
    {
    public:
        PlayerInventory();

        void render() override;
        void update() override;
    private:
        int width,height;
    };
}
