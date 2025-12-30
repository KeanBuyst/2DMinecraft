#pragma once
#include "UI.h"

#include "../Util.h"

namespace UI
{
    class Inventory : public UIComponent
    {
    public:
        Inventory(glm::vec2 position, uint8_t width, uint8_t height);
        Inventory(const Inventory& other) = delete;
        ~Inventory() override;

        bool addItem(world::Item* item);
        void setItem(int index,world::Item* item);

        world::Item* getItem(int index);

        glm::vec2 getPosition() const override;
        void setPosition(glm::vec2 position);
        glm::ivec2 getSize() const override;
        int getCount() const override;
        const Cell* getCells() const override;

        virtual void serialize(Util::ByteStream& stream);
        virtual void load(Util::ByteStream& stream);

        void update() override;

        bool isVisible() const override;
        void setVisible(bool visible) override;
    protected:
        Cell& at(int x, int y);
        Cell* GetMouseSlot(int& index);

        glm::vec2 position;
        uint8_t width,height;
        bool updated;
        bool visible;
        Cell* cells;
    };

    class Hotbar : public Inventory
    {
    private:
        int selected_slot;
    public:
        Hotbar();

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

        void update() override;
    };
}
