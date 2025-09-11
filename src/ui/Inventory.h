#pragma once
#include "UI.h"

namespace UI
{
    struct Inventory : UIComponent
    {
    public:
        Inventory(glm::vec2 position, int width, int height);
        ~Inventory();

        bool addItem(world::Item* item) const;

        glm::vec2 getPosition() const override;
        void setPosition(glm::vec2 position);
        glm::ivec2 getSize() const override;
        int getCount() const override;
        const Cell* getCells() const override;

        void update(const float& delta_time) override;

        bool isVisible() const override;
        void setVisible(bool visible) override;
    protected:
        Cell& at(int x, int y);

        glm::vec2 position;
        const int width,height;
        bool visible;
        Cell* cells;
    };

    struct Hotbar : Inventory
    {
        Hotbar();

        void update(const float& delta_time) override;
        void setSelectedSlot(int slot);
        world::Item*& getSelectedItem() const;

    private:
        int selected_slot;
    };

    struct MouseItemHolder : Inventory
    {
        MouseItemHolder();
        void update(const float& delta_time) override;
        void setItem(world::Item* item) const;
        world::Item* getItem() const;
    };

    struct PlayerInventory : Inventory
    {
        PlayerInventory();
    };
}
